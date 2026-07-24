// Enemy.cpp
#include "Enemy.h"
#include "Player.h"
#include "Props.h"
#include <cmath>

// กำหนดค่าเริ่มต้นให้ซอมบี้เมื่อถูกเสกออกมา
Enemy::Enemy(float startX, float startY, float moveSpeed) {
    id = GetRandomValue(100000, 999999);
    x = startX;
    y = startY;
    width = 50.0f;
    height = 50.0f;
    active = true;
    speed = moveSpeed;
    facingRight = false;
    velocityY = 0.0f;
    currentFrame = 0;
    frameCounter = 0;
    hp = 4;
    maxHp = 4;
    attackCooldownTimer = 0;
    state = ZombieState::WALK;
    stateTimer = 0;
}

void Enemy::Draw(AssetManager& assets) {
    if (!active) return;

    Texture2D* currentTex = &assets.zombieWalkSheet;
    int totalFrames = 10; 
    
    if (state == ZombieState::IDLE) { currentTex = &assets.zombieIdleSheet; totalFrames = 6; }
    else if (state == ZombieState::WALK) { currentTex = &assets.zombieWalkSheet; totalFrames = 10; }
    else if (state == ZombieState::ATTACK) { currentTex = &assets.zombieAttackSheet; totalFrames = 5; }
    else if (state == ZombieState::HURT) { currentTex = &assets.zombieHurtSheet; totalFrames = 4; }
    else if (state == ZombieState::DEAD) { currentTex = &assets.zombieDeathSheet; totalFrames = 5; }

    float frameWidth = (float)currentTex->width / (float)totalFrames; 
    float totalHeight = (float)currentTex->height;
    float sourceX = currentFrame * frameWidth;
    
    Rectangle srcRec = { sourceX, 0, frameWidth, totalHeight };
    if (!facingRight) srcRec.width = -srcRec.width; 
    
    float visualOffset = -45.0f; 
    float centerY = y + (height / 2.0f);
    float drawY = centerY + visualOffset;
    float destWidth = frameWidth * 1.25f; 
    float destHeight = totalHeight * 1.25f;
    Rectangle destRec = { x + (width / 2.0f), drawY, destWidth, destHeight };

    Color drawColor = WHITE;
    if (state == ZombieState::DEAD && currentFrame == 4) {
        if (stateTimer < 60) { // 1 วินาทีสุดท้ายก่อนหาย (60 เฟรม)
            float alpha = (float)stateTimer / 60.0f; // จะได้ค่า 1.0 ลดลงไปถึง 0.0
            drawColor = Fade(WHITE, alpha); // ทำให้ศพจางลง
        }
    }
    
    DrawTexturePro(*currentTex, srcRec, destRec, { destWidth / 2.0f, destHeight / 2.0f }, 0.0f, drawColor);
    
    // วาดหลอดเลือดเหนือหัวซอมบี้
    if (state != ZombieState::DEAD) {
        float barX = x + (width / 2.0f) - 20.0f; 
        DrawRectangle(barX, y - 30.0f, 40.0f, 6.0f, DARKGRAY);
        float hpPercent = (float)hp / (float)maxHp;
        Color hpColor = LIME; 
        if (hpPercent <= 0.6f) hpColor = ORANGE; 
        if (hpPercent <= 0.35f) hpColor = RED;   
        DrawRectangle(barX, y - 30.0f, 40.0f * hpPercent, 6.0f, hpColor);
    }
}

void Enemy::Update(Player& player, const std::vector<Rectangle>& platforms, float gravity, std::vector<Item>& items, AssetManager& assets) {
    if (!active) return;

    // เช็คตาย
    if (hp <= 0 && state != ZombieState::DEAD) {
        state = ZombieState::DEAD;
        currentFrame = 0; 
        PlaySound(assets.deathSound);
        stateTimer = 1200; // ตั้งเวลาลบศพ
    }

    if (state == ZombieState::DEAD) {
        if (currentFrame < 4) {
            frameCounter++;
            if (frameCounter >= 6) { 
                frameCounter = 0;
                currentFrame++;
                // สุ่มดรอปไอเทมตอนตาย
                if (currentFrame == 4) { 
                    if (GetRandomValue(1, 100) <= 60) {
                        int randType = 0;
                        int chance = GetRandomValue(1, 100);
                        if (chance <= 60) randType = 0; else if (chance <= 90) randType = 1; else randType = 2;
                        items.push_back(Item(x + width/2 - 10, y + height - 20, randType));
                    }
                }
            }
        } 
        else {
            // ถ้าแอนิเมชันตายเล่นจบแล้ว
            if (stateTimer > 0) stateTimer--;
            if (stateTimer <= 0) active = false; // หมดเวลา ปิดการทำงานเตรียมลบจาก RAM
        }
    }
    
    else if (state == ZombieState::HURT) {
        stateTimer--;
        if (stateTimer <= 0) state = ZombieState::WALK;

        frameCounter++;
        if (frameCounter >= 6) { 
            frameCounter = 0; currentFrame++; 
            if (currentFrame >= 4) currentFrame = 0; 
        }
    }
    else if (state == ZombieState::ATTACK) {
        stateTimer--;
        if (stateTimer <= 0) state = ZombieState::WALK;

        frameCounter++;
        if (frameCounter >= 5) { 
            frameCounter = 0; currentFrame++; 
            if (currentFrame >= 5) currentFrame = 0; 
        }
    }
    else { 
        // ฟิสิกส์ (แรงโน้มถ่วง)
        velocityY += gravity;
        y += velocityY;
        for (const auto& box : platforms) {
            bool isInsideX = (x + width > box.x) && (x < box.x + box.width);
            bool isFallingOnTop = (y + height >= box.y) && (y + height <= box.y + velocityY + 5.0f) && (velocityY >= 0);
            if (isInsideX && isFallingOnTop) { y = box.y - height; velocityY = 0; }
        }

        // AI Logic: Pathfinding & Edge Detection
        float distX = player.x - x;
        if (std::abs(distX) < 600.0f) {
            state = ZombieState::WALK;
            
            float nextX = x;
            
            if (std::abs(distX) > 5.0f) { 
                if (distX > 0) { nextX += speed; facingRight = true; } 
                else { nextX -= speed; facingRight = false; }
            }
            
            float checkX = facingRight ? (nextX + width) : nextX;
            bool groundAhead = false;
            
            for (const auto& box : platforms) {
                if (checkX >= box.x && checkX <= box.x + box.width && 
                    (y + height >= box.y - 5.0f && y + height <= box.y + 10.0f)) {
                    groundAhead = true;
                    break; 
                }
            }
            
            if (groundAhead) { x = nextX; } else { state = ZombieState::IDLE; }
            
        } else {
            state = ZombieState::IDLE;
        }

        frameCounter++;
        if (frameCounter >= 8) { 
            frameCounter = 0;
            currentFrame++;
            int maxF = (state == ZombieState::WALK) ? 10 : 6; 
            if (currentFrame >= maxF) currentFrame = 0; 
        }

        if (attackCooldownTimer > 0) attackCooldownTimer--;
        Rectangle enemyRect = { x, y, width, height };
        Rectangle playerRect = { player.x, player.y, player.width, player.height };
        
        // กัดผู้เล่น
        if (CheckCollisionRecs(playerRect, enemyRect)) {
            if (attackCooldownTimer <= 0) {
                state = ZombieState::ATTACK;
                stateTimer = 30; 
                currentFrame = 0;
                attackCooldownTimer = 90;

                if (player.iframeTimer <= 0) {
                    player.hp--; 
                    player.iframeTimer = 8; 
                    attackCooldownTimer = 90;
                    PlaySound(assets.playerHurtSound);
                    
                    player.targetHp = player.hp; 
                    player.displayHp = player.hp;

                    if (player.hp <= 0) { 
                        player.isAlive = false; 
                        player.deathCause = "DEVOURED BY ZOMBIES"; // โดนซอมบี้รุมทึ้ง
                    }
                }
            }
        }
    }
}