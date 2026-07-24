// Props.cpp
#include "Props.h"
#include "AssetManager.h"
#include <cmath>

// สร้างกระสุน
Bullet::Bullet(float startX, float startY, float dx, float dy, int bulletType, bool enemyBullet) {
    x = startX;
    y = startY;
    type = bulletType;
    isEnemyBullet = enemyBullet;

    if (type == 1) { 
        width = 20.0f; height = 20.0f; 
    } else if (type == 2) { 
        // ขนาดลูกไฟ
        width = 45.0f; height = 45.0f; 
    } else { 
        width = 10.0f; height = 10.0f; 
    }
    
    speedX = dx;
    speedY = dy;
    active = true;

    angle = (atan2(dy, dx) * (180.0f / PI));
    if (type != 2) { 
        angle += 90.0f; 
    }

    currentFrame = 0;
    frameCounter = 0;
    isHit = false;
}

// function อนิเมชั่นกระสุน
void Bullet::Update() {
    if (!active) return;

    if (!isHit) {
        // สถานะ: กำลังบิน
        x += speedX;
        y += speedY;

        if (type == 0) { // แอนิเมชันบินปืนพก (รูป 1-4)
            if (currentFrame < 3) {
                frameCounter++;
                if (frameCounter >= 2) { 
                    frameCounter = 0;
                    currentFrame++;
                }
            }
        }
        // ลอจิกสับเฟรมแอนิเมชันลูกไฟ (รูป 1-5) วนลูปไปเรื่อยๆ ขณะบิน
        else if (type == 2) { 
            frameCounter++;
            if (frameCounter >= 4) { // ปรับเลข 4 เพื่อเพิ่ม/ลดความเร็วของการหมุนลูกไฟ
                frameCounter = 0;
                currentFrame++;
                if (currentFrame >= 5) currentFrame = 0; // มี 5 รูป ให้วนกลับมาภาพแรก
            }
        }
    } else {
        // สถานะ: ชนเป้าหมาย
        if (type == 0) { 
            frameCounter++;
            if (frameCounter >= 4) { 
                frameCounter = 0;
                currentFrame++;
                if (currentFrame > 5) active = false; 
            }
        }
    }
}

// function การชนเป้าหมาย
void Bullet::Hit() {
    if (isHit) return;
    isHit = true;

    if (type == 0) { 
        currentFrame = 4; // กระโดดไปใช้ภาพกระสุนแตกภาพแรก (index 4)
        frameCounter = 0;
        speedX = 0; // หยุดวิ่ง
        speedY = 0; 
    } else { 
        active = false; // RPG ทำลายทิ้งเลยเพราะมันเรียกระเบิดวงใหญ่แทน
    }
}

// =======================================================
// ระบบถังไม้ Crate โฉมใหม่
// =======================================================
Crate::Crate(float startX, float startY) {
    x = startX;
    y = startY;
    width = 40.0f;  
    height = 40.0f;
    hp = 5;
    active = true;
    
    isBroken = false;
    hitFlashTimer = 0;
    brokenTimer = 0;
    expFrame = 0;
    expTimer = 0;
    offsetTopY = 0.0f;
    offsetBottomY = 0.0f;
}

// =======================================================
// ดึงขนาด Hitbox ของกล่อง
// =======================================================
Rectangle Crate::GetHitbox() const {
    float hitScaleX = 0.5f; 
    float hitScaleY = 1.0f; 
    
    float hitWidth = width * hitScaleX;
    float hitHeight = height * hitScaleY;
    
    // คำนวณให้อยู่กึ่งกลางพิกัด x, y เดิม
    float offsetX = (width - hitWidth) / 2.0f;
    float offsetY = (height - hitHeight) / 2.0f;
    
    return { x + offsetX, y + offsetY, hitWidth, hitHeight };
}

void Crate::Update() {
    if (!active) return;
    
    // ลดเวลาเวลาโดนยิงแล้วกระพริบแดง
    if (hitFlashTimer > 0) hitFlashTimer--;

    // ถ้ากล่องแตกแล้ว ให้รันแอนิเมชันกล่องพัง
    if (isBroken) {
        brokenTimer--; // ลดเวลาจนกว่าจะลบออกจากจอ
        
        // ให้เศษสองชิ้นค่อยๆ แยกออกจากกัน
        offsetTopY -= 0.5f; 
        offsetBottomY += 0.5f;

        // รันแอนิเมชันเอฟเฟกต์ระเบิด (12 ภาพ)
        if (expFrame < 12) {
            expTimer++;
            if (expTimer >= 3) { // ปรับความรัวของระเบิด
                expTimer = 0;
                expFrame++;
            }
        }

        // พอเวลาหมด (1 วินาที) ก็ปิด active ทิ้งไป
        if (brokenTimer <= 0) active = false;
    }
}

void Crate::Draw(AssetManager& assets) {
    if (!active) return;
    
    // 1. สถานะกล่องปกติ (ยังไม่แตก)
    if (!isBroken) {
        Texture2D tex = (hp >= 3) ? assets.crateTex[0] : assets.crateTex[1];
        Color drawCol = (hitFlashTimer > 0) ? RED : WHITE;
        
        // สูตรคำนวณสัดส่วนภาพจริง ป้องกันภาพอ้วน/บี้ (Aspect Ratio)
        float drawHeight = ((float)tex.height / (float)tex.width) * width;
        
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        // ใช้ drawHeight ที่คำนวณมาใหม่แทน height เดิม
        Rectangle dest = { x + width / 2.0f, y + drawHeight / 2.0f, width, drawHeight };
        DrawTexturePro(tex, src, dest, { width / 2.0f, drawHeight / 2.0f }, 0.0f, drawCol);
    }
    // 2. สถานะกล่องแตกหัก
    else {
        float alpha = (float)brokenTimer / 60.0f; 
        bool blink = (brokenTimer / 5) % 2 == 0;
        Color fadeCol = Fade(WHITE, blink ? alpha : alpha * 0.5f);

        // ==========================================
        // วาดชิ้นบน 
        // ==========================================
        Texture2D texTop = assets.crateTex[2];

        float topScale = 1.2f; 
        float finalWidthTop = width * topScale;
        float drawHeightTop = ((float)texTop.height / (float)texTop.width) * finalWidthTop; 
        
        Rectangle srcTop = { 0, 0, (float)texTop.width, (float)texTop.height };
        Rectangle destTop = { x + width / 2.0f, y + (drawHeightTop / 2.0f) + offsetTopY, finalWidthTop, drawHeightTop };
        DrawTexturePro(texTop, srcTop, destTop, { finalWidthTop / 2.0f, drawHeightTop / 2.0f }, 0.0f, fadeCol);

        // ==========================================
        // วาดชิ้นล่าง
        // ==========================================
        Texture2D texBot = assets.crateTex[3];

        // ปรับขนาดชิ้นล่าง
        float botScale = 1.2f; 
        float finalWidthBot = width * botScale;
        float drawHeightBot = ((float)texBot.height / (float)texBot.width) * finalWidthBot; 
        
        Rectangle srcBot = { 0, 0, (float)texBot.width, (float)texBot.height };
        Rectangle destBot = { x + width / 2.0f, (y + height) - (drawHeightBot / 2.0f) + offsetBottomY, finalWidthBot, drawHeightBot };
        DrawTexturePro(texBot, srcBot, destBot, { finalWidthBot / 2.0f, drawHeightBot / 2.0f }, 0.0f, fadeCol);

        // ==========================================
        // วาดเอฟเฟกต์ระเบิด (ไว้ตรงกลาง)
        // ==========================================
        if (expFrame < 12) {
            Texture2D expTex = assets.crateExpTex[expFrame];
            float scale = 1.6f; 
            Rectangle expSrc = { 0, 0, (float)expTex.width, (float)expTex.height };
            Rectangle expDest = { x + width / 2.0f, y + height / 2.0f, expTex.width * scale, expTex.height * scale };
            DrawTexturePro(expTex, expSrc, expDest, { (expTex.width * scale) / 2.0f, (expTex.height * scale) / 2.0f }, 0.0f, WHITE);
        }
    }
    // DrawRectangleLinesEx(GetHitbox(), 2, MAGENTA); // วาดกรอบ Hitbox สีม่วง (debug)
}

// สร้างไอเทม
Item::Item(float startX, float startY, int itemType) {
    id = GetRandomValue(10000, 99999);
    x = startX;
    y = startY;
    width = 35.0f;
    height = 35.0f;
    type = itemType;
    active = true;
    lifeTimer = 360;

    currentFrame = 0;
    frameCounter = 0;
}

// จัดการ Animation ของ items
void Item::Update() {
    if (!active) return;

    if (type == 0) { // ถ้าเป็นเหรียญ (type 0) ให้เล่นแอนิเมชัน
        frameCounter++;
        if (frameCounter >= 4) { // ปรับความเร็วการหมุนตรงนี้
            frameCounter = 0;
            currentFrame++;
            if (currentFrame >= 8) { // มี 8 ภาพ
                currentFrame = 0;
            }
        }
    }
}

void Item::Draw(AssetManager& assets) {
    if (!active) return; // ถ้าไอเทมไม่ active ไม่ต้องวาด
    
    // เอฟเฟกต์กระพริบเมื่อไอเทมใกล้หมดเวลา (Blinking Effect)
    bool shouldDraw = true;
    if (type != 0 && lifeTimer <= 180) {
        if ((lifeTimer / 10) % 2 != 0) shouldDraw = false;
    }

    if (shouldDraw) {
        if (type == 0) { // สมบัติ
            Texture2D tex = assets.coinTex;
            float frameWidth = (float)tex.width / 8.0f;
            float frameHeight = (float)tex.height;

            Rectangle src = { currentFrame * frameWidth, 0, frameWidth, frameHeight };
            
            float scale = 1.5f; // ปรับความใหญ่ของเหรียญ
            Rectangle dest = { x + (width - (frameWidth * scale)) / 2.0f, 
                               y + (height - (frameHeight * scale)) / 2.0f, 
                               frameWidth * scale, frameHeight * scale };
            
            DrawTexturePro(tex, src, dest, {0, 0}, 0.0f, WHITE);
        }
        else if (type == 1) { // กระสุน
            float imgWidth = 45.0f;  
            float imgHeight = 45.0f; 
            Rectangle src = { 0, 0, (float)assets.ammoBoxTex.width, (float)assets.ammoBoxTex.height };
            Rectangle dest = { x + (width - imgWidth) / 2.0f, y + (height - imgHeight), imgWidth, imgHeight };
            DrawTexturePro(assets.ammoBoxTex, src, dest, {0, 0}, 0.0f, WHITE);
        }
        else if (type == 2) { // ยาพยาบาล
            float imgWidth = 45.0f;  
            float imgHeight = 45.0f; 
            Rectangle src = { 0, 0, (float)assets.medkitTex.width, (float)assets.medkitTex.height };
            Rectangle dest = { x + (width - imgWidth) / 2.0f, y + (height - imgHeight), imgWidth, imgHeight };
            DrawTexturePro(assets.medkitTex, src, dest, {0, 0}, 0.0f, WHITE);
        }
    }
}

void Bullet::Draw(AssetManager& assets) {
    if (!active) return;

    Texture2D tex;
    float scale = 1.0f; // ตัวแปรตั้งต้น

    if (type == 1) { // RPG
        if (isHit) return; 
        tex = assets.rpgRocketTex;
        scale = 1.0f;
    } 
    else if (type == 2) { //  ลูกไฟ
        if (isHit) return;
        tex = assets.fireballTex[currentFrame % 5];
        scale = 2.5f; // ขนาดลูกไฟ
    }
    else { // ปืนพก
        tex = assets.pistolBulletTex[currentFrame];
        scale = 1.0f;
    }

    Rectangle srcRec = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle destRec = { x, y, tex.width * scale, tex.height * scale };
    Vector2 origin = { (tex.width * scale) / 2.0f, (tex.height * scale) / 2.0f };

    DrawTexturePro(tex, srcRec, destRec, origin, angle, WHITE);
}