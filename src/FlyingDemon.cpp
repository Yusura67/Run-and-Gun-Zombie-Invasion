// FlyingDemon.cpp
#include "FlyingDemon.h"
#include <cmath>

FlyingDemon::FlyingDemon(float startX, float startY) {
    x = startX;
    y = startY;
    width = 60.0f;
    height = 60.0f;
    speed = 2.5f;
    maxHp = 6;
    hp = maxHp;
    active = true;
    facingRight = false;
    
    state = DemonState::FLYING;
    currentFrame = 0;
    frameCounter = 0;
    stateTimer = 0;
    attackCooldown = 60; // ดีเลย์ก่อนพ่นไฟครั้งแรก
}

void FlyingDemon::Update(Player& player, std::vector<Bullet>& bullets, AssetManager& assets) {
    if (!active) return;

    // 1. เช็คตาย
    if (hp <= 0 && state != DemonState::DEAD) {
        state = DemonState::DEAD;
        currentFrame = 0;
        frameCounter = 0;
        PlaySound(assets.demonScreechSound); 
    }

    if (state == DemonState::DEAD) {
        frameCounter++;
        if (frameCounter >= 6) { // ความเร็วการเล่นแอนิเมชันตอนตาย
            frameCounter = 0;
            currentFrame++;
            if (currentFrame >= 7) { 
                active = false; // หายวับไปทันที ไม่ทิ้งซากเฟรมสุดท้ายค้างไว้บนจอ
            }
        }
        return; 
    }
    
    else if (state == DemonState::HURT) {
        stateTimer--;
        frameCounter++;
        if (frameCounter >= 5) {
            frameCounter = 0;
            currentFrame++;
            if (currentFrame >= 4) currentFrame = 3; 
        }
        if (stateTimer <= 0) state = DemonState::FLYING;
    }
    
    else if (state == DemonState::ATTACK) {
        frameCounter++;
        if (frameCounter >= 6) { 
            frameCounter = 0;
            currentFrame++;

            // จังหวะที่ 4 อ้าปากพ่นไฟ
            if (currentFrame == 4) {
                float mouthX = facingRight ? (x + width - 10.0f) : (x + 10.0f);
                float mouthY = y + (height * 0.4f);
                
                // คณิตศาสตร์การคำนวณทิศทาง เล็งเป้าหมายเฉียงลงมาจากฟ้าพุ่งตรงไปที่กลางตัวผู้เล่น
                float dx = (player.x + player.width / 2.0f) - mouthX;
                float dy = (player.y + player.height / 2.0f) - mouthY;
                float length = sqrt(dx * dx + dy * dy);
                
                if (length > 0) {
                    dx /= length; 
                    dy /= length;
                }
                
                float fireSpeed = 6.5f; // ความเร็วของลูกไฟ

                // เสกลูกไฟแบบระบุเวกเตอร์เฉียงตามจริง
                bullets.push_back(Bullet(mouthX, mouthY, dx * fireSpeed, dy * fireSpeed, 2, true));
                PlaySound(assets.demonFireSound);
            }

            if (currentFrame >= 8) { 
                state = DemonState::FLYING;
                currentFrame = 0;
                attackCooldown = GetRandomValue(90, 150); // สุ่มคูลดาวน์พ่นไฟรอบถัดไป
            }
        }
    }
    else {
        // =======================================================
        // AI บินลอยอยู่บนฟ้า
        // =======================================================
        if (attackCooldown > 0) attackCooldown--;

        float distX = player.x - x;
        facingRight = (distX > 0);

        // คำนวณจุดเล็งบิน
        float targetX = player.x + (facingRight ? -250.0f : 250.0f); // รักษาระยะห่างแนวนอน
        float targetY = player.y - 200.0f; // 👈 บินลอยสูงเด่นอยู่บนฟ้าตลอดเวลา

        targetY += sinf(GetTime() * 3.0f) * 15.0f;

        // สั่งให้เคลื่อนที่เข้าหาเป้าหมายบนฟ้า
        x += (targetX - x) * 0.04f;
        y += (targetY - y) * 0.04f;

        // ระยะห่างจากเป้าหมายในแนวนอน
        if (std::abs(distX) < 450.0f && attackCooldown <= 0) {
            state = DemonState::ATTACK;
            currentFrame = 0;
            frameCounter = 0;
        }

        // แอนิเมชันตอนบินปกติ
        frameCounter++;
        if (frameCounter >= 6) {
            frameCounter = 0;
            currentFrame++;
            if (currentFrame >= 4) currentFrame = 0;
        }
    }
}

void FlyingDemon::Draw(AssetManager& assets) {
    if (!active) return;

    Texture2D* currentTex = &assets.fdFlyingTex;
    int totalFrames = 4;
    
    if (state == DemonState::IDLE) { currentTex = &assets.fdIdleTex; totalFrames = 4; }
    else if (state == DemonState::FLYING) { currentTex = &assets.fdFlyingTex; totalFrames = 4; }
    else if (state == DemonState::ATTACK) { currentTex = &assets.fdAttackTex; totalFrames = 8; }
    else if (state == DemonState::HURT) { currentTex = &assets.fdHurtTex; totalFrames = 4; }
    else if (state == DemonState::DEAD) { currentTex = &assets.fdDeathTex; totalFrames = 7; }

    if (currentTex->id == 0) return; 

    float frameWidth = (float)currentTex->width / (float)totalFrames;
    float totalHeight = (float)currentTex->height;
    
    int safeFrame = currentFrame % totalFrames;
    float sourceX = safeFrame * frameWidth;

    Rectangle srcRec = { sourceX, 0, frameWidth, totalHeight };
    if (facingRight) srcRec.width = -srcRec.width; 

    float scale = 1.5f;
    float destWidth = frameWidth * scale;
    float destHeight = totalHeight * scale;
    Rectangle destRec = { x + width/2.0f, y + height/2.0f, destWidth, destHeight };

    DrawTexturePro(*currentTex, srcRec, destRec, { destWidth / 2.0f, destHeight / 2.0f }, 0.0f, WHITE);

    // วาดหลอดเลือด
    if (state != DemonState::DEAD) {
        float barX = x + (width / 2.0f) - 20.0f; 
        DrawRectangle(barX, y - 15.0f, 40.0f, 5.0f, DARKGRAY);
        float hpPercent = (float)hp / (float)maxHp;
        Color hpColor = LIME; 
        if (hpPercent <= 0.5f) hpColor = ORANGE; 
        if (hpPercent <= 0.2f) hpColor = RED;   
        DrawRectangle(barX, y - 15.0f, 40.0f * hpPercent, 5.0f, hpColor);
    }
}