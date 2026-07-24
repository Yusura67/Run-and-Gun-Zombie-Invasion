// Player.cpp
#include "Player.h"
#include <cmath>

void Player::Reset() {
    // =================================================
    // พิกัดและขนาดตัว
    // =================================================
    x = 100.0f;
    y = 200.0f;
    width = 40.0f;
    height = 40.0f;
    // =================================================

    // =================================================
    // สถานะพื้นฐาน
    // =================================================
    hp = 10;
    maxHp = 10;
    targetHp = 10; 
    displayHp = 10.0f;
    speed = 5.0f;
    velocityY = 0.0f;
    facingRight = true;
    isAlive = true;
    isWinner = false;
    score = 0;
    // =================================================

    // =================================================
    // ระบบปืนและกระสุน
    // =================================================
    weaponType = 0;

    // [0] ปืนพกปกติ
    ammo[0] = 15;
    maxAmmo[0] = 15;
    reserveAmmo[0] = 45;

    // [1] ปืน RPG 
    ammo[1] = 1;        // ในกระบอกมีแค่ 1 นัด
    maxAmmo[1] = 2;     // ยิงปุ๊บต้องรีโหลดปั๊บ
    reserveAmmo[1] = 2;

    isReloading = false;
    reloadTimer = 0;
    outOfAmmoTimer = 0;
    shootCooldownTimer = 0;
    rotation = 0.0f;
    recoilAngle = 0.0f;

    isMoving = false;
    isLegFacingRight = true;
    currentLegFrame = 0;
    legFrameCounter = 0;

    isDrinking = false;
    currentDrinkFrame = 0;
    drinkFrameCounter = 0;
    
    // =================================================
    
    // ระบบอมตะ
    iframeTimer = 0;
    deathCause = "";
}

void Player::Draw(AssetManager& assets) {
    if (!isAlive) return;

    float centerX = x + width / 2.0f;
    float centerY = y + height / 2.0f;

    Color playerColor = WHITE;
    if (iframeTimer > 0) { if ((iframeTimer / 5) % 2 == 0) playerColor = RED; }

    // ==============================================================
    // 1. โหลดรูปท่อนล่าง (ระบบกระโดด 3 เฟรม)
    // ==============================================================
    Texture2D currentLegTex;

    if (isDrinking) {
        Texture2D tex = assets.playerDrinkTex[currentDrinkFrame];
        float scale = 1.1f; // สเกล
        float dWidth = tex.width * scale;
        float dHeight = tex.height * scale;
        
        // วาดให้จุดศูนย์กลางอยู่ตรงกลางเท้าด้านล่างเหมือนระบบบอสและตัวปกติของผู้เล่น
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        if (facingRight) src.width = -src.width; // พลิกซ้ายขวาตามเมาส์/ทิศทางที่เครื่องบินหันมา
        
        Rectangle dest = { x + width / 2.0f, y + height, dWidth, dHeight };
        Vector2 origin = { dWidth / 2.0f, dHeight };
        
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
        return; // ออกจากฟังก์ชันเลย ไม่วาดท่อนบนท่อนล่างปกติ
    }

    if (!isGrounded) { 
        int jumpIndex = 0;
        
        if (velocityY < -1.5f) {
            jumpIndex = 0; // ขาขึ้น
        } 
        else if (velocityY < 1.5f) {
            jumpIndex = 1; // ลอยตัวจุดสูงสุด
        } 
        else {
            jumpIndex = 2; // ขาลง
        }
        
        currentLegTex = assets.playerLegsJump[jumpIndex];
    } 
    else if (isMoving) {
        currentLegTex = assets.playerLegsWalk[currentLegFrame];
    } 
    else {
        currentLegTex = assets.playerLegsIdle;
    }

    // ==============================================================
    // 2. คำนวณเลือกระดับภาพท่อนบน (Aiming Direction)
    // ==============================================================
    // แปลง rotation (ค่าจาก -180 ถึง 180) ให้อยู่ในมุมมองของตัวละคร
    float pitch = rotation;
    if (!facingRight) {
        if (pitch > 0) pitch = 180.0f - pitch;
        else pitch = -180.0f - pitch;
    }

    int bodyIndex = 3; // ค่าเริ่มต้นคือ [3] มองตรง

    // เช็คองศาแล้วชี้ไปที่รูปภาพ
    if (pitch <= -55.0f)      bodyIndex = 0; // [0] มองบนประมาณ 60 องศา
    else if (pitch <= -35.0f) bodyIndex = 1; // [1] มองบน 45 องศา
    else if (pitch <= -10.0f) bodyIndex = 2; // [2] มองบนนิดหน่อย
    else if (pitch < 10.0f)   bodyIndex = 3; // [3] มองตรง (สวิงขึ้นลงไม่เกิน 10 องศา)
    else if (pitch < 35.0f)   bodyIndex = 4; // [4] ก้มลงนิด
    else if (pitch < 55.0f)   bodyIndex = 5; // [5] ก้มลงอีก
    else                      bodyIndex = 6; // [6] ก้มลงมาก

    // ==============================================================
    // 3. โหลดรูปภาพและตั้งค่าสเกล
    // ==============================================================
    
    // ท่อนบน
    Texture2D currentBodyTex = assets.bodyTex[bodyIndex];

    // เลือกแขนตามอาวุธ
    Texture2D currentArmTex = (weaponType == 1) ? assets.armRpgTex : assets.armPistolTex;

    // ตั้งสเกล
    float legScale = 1.1f; 
    float bodyScale = 1.1f;
    float armScale = 1.1f;

    float legDrawWidth = currentLegTex.width * legScale; 
    float legDrawHeight = currentLegTex.height * legScale;
    Vector2 legOrigin = { legDrawWidth / 2.0f, legDrawHeight / 2.0f }; 

    float bodyDrawWidth = currentBodyTex.width * bodyScale;
    float bodyDrawHeight = currentBodyTex.height * bodyScale;
    Vector2 bodyOrigin = { bodyDrawWidth / 2.0f, bodyDrawHeight / 2.0f }; 

    // ขนาดแขน
    float armDrawWidth = currentArmTex.width * armScale;
    float armDrawHeight = currentArmTex.height * armScale;

    // ==============================================================
    // 4. ตั้งค่าตำแหน่ง (Offset) และวาดภาพ
    // ==============================================================
    float legOffsetY = 15.0f;   
    float bodyOffsetY = -15.0f; 

    float legOffsetX = -5.0f;
    float bodyOffsetX = 0.0f;

    float finalLegCenterX = centerX;
    float finalBodyCenterX = centerX;

    if (facingRight) {
        // หันขวา ขยับตามแกน X ปกติ
        finalLegCenterX += legOffsetX;
        finalBodyCenterX += bodyOffsetX;
    } else {
        // หันซ้าย ต้องสลับทิศทางการขยับ
        finalLegCenterX -= legOffsetX;
        finalBodyCenterX -= bodyOffsetX;
    }

    // วาดขาท่อนล่าง
    Rectangle legDestRec = { finalLegCenterX, centerY + legOffsetY, legDrawWidth, legDrawHeight }; 
    Rectangle legsSource = { 0, 0, (float)currentLegTex.width, (float)currentLegTex.height };
    
    // (ขาพลิกซ้าย-ขวาตามปุ่มเดิน)
    if (isLegFacingRight) { legsSource.width = -legsSource.width; } 
    DrawTexturePro(currentLegTex, legsSource, legDestRec, legOrigin, 0.0f, playerColor);

    // วาดตัวท่อนบน
    Rectangle bodyDestRec = { finalBodyCenterX, centerY + bodyOffsetY, bodyDrawWidth, bodyDrawHeight }; 
    Rectangle bodySource = { 0, 0, (float)currentBodyTex.width, (float)currentBodyTex.height };
    
    // (ตัวพลิกซ้าย-ขวาตามเมาส์)
    if (!facingRight) { bodySource.width = -bodySource.width; } 
    DrawTexturePro(currentBodyTex, bodySource, bodyDestRec, bodyOrigin, 0.0f, playerColor);

    // ==============================================================
    // 5. ระบบแขนหมุน
    // ==============================================================
    
    float shoulderOffsetX = 0.0f;   
    float shoulderOffsetY = 0.0f;   
    Vector2 armOrigin = { 0.0f, 0.0f }; 

    if (weaponType == 1) {
        //  ---------------- ตั้งค่าแขน RPG ---------------- 
        
        // จุดหมุน (Pivot) วัดจากไฟล์รูปจริงที่พิกเซล
        armOrigin = { 38.0f * armScale, 14.0f * armScale }; 
        
        // 2. ขยับจุด ตะปู บนลำตัว
        shoulderOffsetX = -10.0f; 
        shoulderOffsetY = 5.0f;   
    } 
    else {
        //  ---------------- ตั้งค่าแขน ปืนพก ---------------- 
        
        // 1️⃣ จุดหมุน (Pivot) วัดจากไฟล์รูปจริงพิกเซล (4, 7)
        armOrigin = { 4.0f * armScale, 7.0f * armScale }; 
        
        // 2️⃣ ขยับจุด ตะปู บนลำตัว
        shoulderOffsetX = -8.0f;   
        shoulderOffsetY = 7.0f;  
    }

    // สลับแกน X ของหัวไหล่เวลาหันซ้าย
    if (!facingRight) shoulderOffsetX = -shoulderOffsetX; 

    // วาดแขนอิงจาก finalBodyCenterX และ bodyOffsetY ให้แนบสนิทกับลำตัวท่อนบน
    Rectangle armDestRec = { finalBodyCenterX + shoulderOffsetX, centerY + bodyOffsetY + shoulderOffsetY, armDrawWidth, armDrawHeight };
    Rectangle armSource = { 0, 0, (float)currentArmTex.width, (float)currentArmTex.height };

    // พลิกปืนบน-ล่างเวลาหันซ้าย (กันปืนตีลังกากลับหัว)
    if (!facingRight) { 
        armSource.height = -armSource.height; 
    } 

    // คำนวณองศาแขน + แรงดีด (Visual Recoil)
    float finalArmRotation = rotation;
    if (facingRight) {
        finalArmRotation -= recoilAngle; // หันขวา ให้อยากแขนกระตุกขึ้น (มุมติดลบ)
    } else {
        finalArmRotation += recoilAngle; // หันซ้าย ให้อยากแขนกระตุกขึ้น (มุมบวก)
    }

    // นำองศาใหม่ (finalArmRotation) มาใช้วาดแทน rotation
    DrawTexturePro(currentArmTex, armSource, armDestRec, armOrigin, finalArmRotation, playerColor);
}

void Player::UpdateMovement(const std::vector<Rectangle>& platforms, float gravity, float jumpForce, const std::vector<Crate>& crates) {
    float prevX = x; // เก็บค่า x เดิมไว้เช็คทิศทางการชน
    
    // Physics: Knockback handling
    x += knockbackX;
    if (abs(knockbackX) > 0.1f) {
        knockbackX *= 0.85f; 
    } else {
        knockbackX = 0;
    }

    // รีเซ็ตสถานะการเดินทุกเฟรมก่อนเช็คปุ่มกด
    isMoving = false;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { 
        x += speed; 
        isMoving = true; 
        isLegFacingRight = true;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { 
        x -= speed; 
        isMoving = true; 
        isLegFacingRight = false;
    }

    // =======================================================
    // 1. เช็คชนกล่องในแกน X (ป้องกันเดินทะลุก้มัน)
    // =======================================================
    Rectangle playerHitboxX = { x, y, width, height };
    for (const auto& c : crates) {
        if (c.active && !c.isBroken) {
            Rectangle cHitbox = c.GetHitbox();
            if (CheckCollisionRecs(playerHitboxX, cHitbox)) {
                if (x > prevX) { // เดินขวาชน
                    x = cHitbox.x - width;
                    knockbackX = 0; // หยุดกระเด็นถ้าติดกล่อง
                } 
                else if (x < prevX) { // เดินซ้ายชน
                    x = cHitbox.x + cHitbox.width;
                    knockbackX = 0;
                }
            }
        }
    }

    // ลอจิกรันอนิเมชั่นเดิน
    if (isMoving) {
        legFrameCounter++;
        if (legFrameCounter >= 4) { 
            legFrameCounter = 0;
            currentLegFrame++;
            if (currentLegFrame >= 8) currentLegFrame = 0;
        }
    } else {
        currentLegFrame = 0;
        legFrameCounter = 0;
    }

    // =======================================================
    // 2. แกน Y (แรงโน้มถ่วง และเช็คเหยียบพื้น / เหยียบกล่อง)
    // =======================================================
    velocityY += gravity; 
    y += velocityY;

    isGrounded = false; 
    
    // 2.1 เช็คเหยียบ Platforms ปกติ
    for (const auto& box : platforms) {
        float footCheckWidth = width * 0.6f;
        float footCheckX = x + (width * 0.2f); 
        bool isInsideX = (footCheckX + footCheckWidth > box.x) && (footCheckX < box.x + box.width);
        float fallTolerance = velocityY + 5.0f; 
        bool isFallingOnTop = (y + height >= box.y) && (y + height <= box.y + fallTolerance) && (velocityY >= 0);
        
        if (isInsideX && isFallingOnTop) {
            y = box.y - height; 
            velocityY = 0;
            isGrounded = true;
        }
    }

    // 2.2 เช็คชนและเหยียบกล่อง (Crates)
    for (const auto& c : crates) {
        if (c.active && !c.isBroken) {
            Rectangle cHitbox = c.GetHitbox();
            
            float footCheckWidth = width * 0.6f;
            float footCheckX = x + (width * 0.2f); 
            bool isInsideX = (footCheckX + footCheckWidth > cHitbox.x) && (footCheckX < cHitbox.x + cHitbox.width);
            float fallTolerance = velocityY + 5.0f; 
            
            // เช็คว่ากำลังตกใส่บนหลังคากล่อง
            bool isFallingOnTop = (y + height >= cHitbox.y) && (y + height <= cHitbox.y + fallTolerance) && (velocityY >= 0);
            
            if (isInsideX && isFallingOnTop) {
                y = cHitbox.y - height; 
                velocityY = 0;
                isGrounded = true;
            }
            // ป้องกันการกระโดดแล้วหัวทะลุทะลวงใต้กล่อง (โหม่งใต้กล่อง)
            else if (isInsideX && velocityY < 0 && y >= cHitbox.y + cHitbox.height && (y + velocityY) <= cHitbox.y + cHitbox.height) {
                y = cHitbox.y + cHitbox.height;
                velocityY = 0; // ชนหัวแล้วร่วงลงมา
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE) && isGrounded) velocityY = jumpForce;
    if (y > 1000) { 
        isAlive = false; 
        hp = 0;
        deathCause = "FELL INTO THE ABYSS"; 
    }
}

void Player::UpdateCombat(Camera2D camera, std::vector<Bullet>& bullets, AssetManager& assets) {
    // ถ้ามีแรงดีดค้างอยู่ ให้ค่อยๆ ลดระดับแขนลงมา (Recovery)
    if (recoilAngle > 0.0f) {
        recoilAngle -= 2.5f; 
        if (recoilAngle < 0.0f) recoilAngle = 0.0f;
    }

    if (IsKeyPressed(KEY_E)) {
        weaponType = (weaponType == 0) ? 1 : 0;
        isReloading = false; // ยกเลิกการรีโหลดทันทีที่เปลี่ยนปืน

        PlaySound(assets.switchWeaponSound);
    }

    float startX = x + (width / 2);
    float startY = y + (height / 2);
    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
    float dx = mouseWorld.x - startX;
    float dy = mouseWorld.y - startY;

    if (mouseWorld.x < startX) facingRight = false;
    else facingRight = true;

    // คำนวณองศาจากเมาส์
    float angle = atan2(dy, dx) * (180.0f / PI);

    // ล็อคการหันขึ้น (Clamp Upward Angle) เฉพาะปืน RPG เท่านั้น
    if (weaponType == 1) { 
        float maxUpAngle = 15.0f; 

        if (facingRight) {
            if (angle < -maxUpAngle && angle >= -90.0f) {
                angle = -maxUpAngle;
            }
        } else {
            if (angle < 0 && angle > -(180.0f - maxUpAngle)) {
                angle = -(180.0f - maxUpAngle);
            }
        }
    }

    rotation = angle;

    // แปลงองศากลับมาเป็นทิศทางการยิง
    float rad = angle * (PI / 180.0f);
    float aimDx = cos(rad); 
    float aimDy = sin(rad);

    int wt = weaponType; // 0 = PISTOL, 1 = RPG

    if (isReloading) {
        reloadTimer--;
        if (reloadTimer <= 0) {
            int ammoNeeded = maxAmmo[wt] - ammo[wt];
            int ammoToLoad = ammoNeeded;
            if (ammoToLoad > reserveAmmo[wt]) ammoToLoad = reserveAmmo[wt];
            ammo[wt] += ammoToLoad;         
            reserveAmmo[wt] -= ammoToLoad;  
            isReloading = false;   
        }
    }

    if (IsKeyPressed(KEY_R) && !isReloading && ammo[wt] < maxAmmo[wt] && reserveAmmo[wt] > 0) {
        isReloading = true;
        reloadTimer = 90; 
        PlaySound(assets.reloadSound); 
    }

    // =======================================================
    // ระบบยิงปืนและแรงดีด
    // =======================================================
    justShot = false;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && shootCooldownTimer <= 0) {
        if (!isReloading) {
            if (ammo[wt] > 0) { 
                if (weaponType == 1) {
                    bullets.push_back(Bullet(startX - 10, startY - 10, aimDx * 8.0f, aimDy * 8.0f, 1));
                    shootCooldownTimer = 45; 
                    PlaySound(assets.rpgShootSound);
                    
                    recoilAngle = 60.0f; // 🎯 ปืน RPG ดีดแรง
                    justShot = true;
                } else {
                    bullets.push_back(Bullet(startX - 5, startY - 5, aimDx * 15.0f, aimDy * 15.0f, 0));
                    shootCooldownTimer = 15; 
                    assets.PlayShootSound();
                    
                    recoilAngle = 15.0f; // 🎯 ปืนพกดีดเบากว่า
                    justShot = true;
                }

                ammo[wt]--; 
            } else if (reserveAmmo[wt] > 0) {
                isReloading = true;
                reloadTimer = 90; 
                PlaySound(assets.reloadSound); 
            } else {
                outOfAmmoTimer = 10;
                PlaySound(assets.emptyClickSound);
                shootCooldownTimer = 15;
            }
        }
    }
}