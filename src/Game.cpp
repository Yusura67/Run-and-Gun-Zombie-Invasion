// Game.cpp
#include "Game.h"
#include <cmath>
#include "../include/rlgl.h"

Game::Game() {
    Init();
}

void Game::Init() {
    InitWindow(screenWidth, screenHeight, "IT Year 3: Run & Gun Zombie Invasion!");
    SetTargetFPS(60);
    InitAudioDevice(); 
    HideCursor(); 

    currentState = GameState::SPLASH;;

    // โหลด Resources ทั้งหมดผ่าน AssetManager
    assets.LoadAll();

    // --- ตั้งค่าหน้าเนื้อเรื่อง ---
    storyTex = LoadTexture("assets/story.png"); 

    Reset();
}

void Game::Reset() {
    player.Reset();
    player2.Reset();
    bullets.clear();
    items.clear();
    platforms.clear();
    flyingDemons.clear();
    floatingTexts.clear();

    // ---------------------------------------------------------
    // Level Design: Platform Generation
    // ---------------------------------------------------------
    
    // Zone 1: ทางเข้าเมืองร้าง
    platforms.push_back({ 0, 500, 500, 200 }); 
    platforms.push_back({ 600, 440, 200, 50 });  
    platforms.push_back({ 900, 380, 200, 50 });  

    // Zone 2: หลุมมรณะ
    platforms.push_back({ 1200, 560, 500, 200 });
    platforms.push_back({ 1810, 560, 500, 200 });

    // Zone 3: หน้าผาสามเหลี่ยม
    platforms.push_back({ 2420, 500, 120, 50 });  
    platforms.push_back({ 2650, 420, 120, 50 });  
    platforms.push_back({ 2880, 480, 120, 50 });  
    platforms.push_back({ 3110, 380, 100, 50 });  
    platforms.push_back({ 3330, 500, 300, 50 });  

    // Zone 4: The Horde Gauntlet
    platforms.push_back({ 3730, 500, 2200, 200 }); 
    platforms.push_back({ 4060, 370, 200, 50 });  // จุดซุ่มยิง 1
    platforms.push_back({ 4800, 370, 200, 50 });  // จุดซุ่มยิง 2

    // Zone 5: ลานประลองซากวิหาร (Boss Arena)
    platforms.push_back({ 6100, 500, 3500, 200 });
    
    // เสาหินสำหรับหลบหลีกการโจมตี Boss Phase 2
    platforms.push_back({ 6700, 350, 250, 50 }); 
    platforms.push_back({ 7400, 350, 250, 50 }); 
    platforms.push_back({ 8100, 350, 250, 50 }); 

    // บัลลังก์บอส (โครงสร้างบันได 3 ขั้น)
    platforms.push_back({ 8800, 450, 800, 200 }); // ขั้นที่ 1
    platforms.push_back({ 8950, 400, 650, 200 }); // ขั้นที่ 2
    platforms.push_back({ 9100, 350, 500, 200 }); // ขั้นที่ 3 (ยอดบัลลังก์)
    
    // กำแพงล่องหนป้องกันผู้เล่นตกฉากด้านขวาสุด
    platforms.push_back({ 9600, -500, 100, 1200 }); 
    
    // ---------------------------------------------------------
    // Boss Status Reset
    // ---------------------------------------------------------
    boss.hp = boss.maxHp;           
    boss.active = false;            
    boss.state = BossState::IDLE;   
    boss.currentFrame = 0;          
    boss.frameCounter = 0;
    boss.attackCooldownTimer = 0;   
    boss.stateTimer = 0;            
    boss.isEnraged = false;         
    
    supplyDropTimer = 0;

    // รีเซ็ตตัวแปรสภาพแวดล้อมและสถานะ Phase 3
    isPhase3Escape = false;   
    friendRedEye = false;     
    friendScale = 1.0f;       
    cameraShakeTimer = 0;     
    screenFlashTimer = 0;

    // ตัวแปรรีเซ็ตเครื่องบินกู้ภัย
    escapePlaneState = 0;
    escapePilotFrame = 0;
    escapePilotTimer = 0;
    
    // ตั้งพิกัดบอส
    boss.x = 9150; 
    boss.y = 100; 
    
    goal = { -1000, -1000, 0, 0 };
    enemies.clear();
    crates.clear();

    // ---------------------------------------------------------
    // Entity Spawning: Crates & Enemies
    // ---------------------------------------------------------
    
    // กล่องเสบียง
    crates.push_back(Crate(200, 450));   
    crates.push_back(Crate(1450, 510));  
    crates.push_back(Crate(2050, 510));  
    crates.push_back(Crate(3500, 450));  
    crates.push_back(Crate(4100, 320));  
    crates.push_back(Crate(6200, 450));

    // ศัตรูทั่วไป
    enemies.push_back(Enemy(700, 390, 1.5f));  
    enemies.push_back(Enemy(1350, 510, 2.0f)); 
    enemies.push_back(Enemy(1550, 510, 1.5f)); 
    enemies.push_back(Enemy(1950, 510, 2.3f)); 
    enemies.push_back(Enemy(2430, 450, 0.8f)); 
    enemies.push_back(Enemy(2890, 430, 1.0f)); 

    // ศัตรูโซน Horde Gauntlet
    enemies.push_back(Enemy(3900, 450, 1.5f)); 
    enemies.push_back(Enemy(4100, 450, 2.5f)); 
    enemies.push_back(Enemy(4250, 450, 1.2f)); 
    enemies.push_back(Enemy(4450, 450, 2.8f)); 
    enemies.push_back(Enemy(4600, 450, 1.7f)); 
    enemies.push_back(Enemy(4800, 450, 2.2f)); 
    enemies.push_back(Enemy(5000, 450, 1.4f)); 
    enemies.push_back(Enemy(5200, 450, 3.0f)); 
    enemies.push_back(Enemy(5500, 450, 1.6f)); 

    // ---------------------------------------------------------
    // Camera Reset
    // ---------------------------------------------------------
    camera = { 0 };
    camera.target = { player.x, player.y };
    camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;

    currentCutscene = CutsceneType::NONE; 
    cutsceneTimer = 0; 
    phase2Triggered = false;
    gameOverSoundPlayed = false;

    currentCutscene = CutsceneType::AIRPLANE_DROP;

    // ตั้งค่าจุดเริ่มเครื่องบิน
    airplaneX = player.x - 50.0f; // อยู่ใกล้ๆ ตัวผู้เล่น
    airplaneY = -300.0f; // เริ่มจากนอกจอด้านบน
    airplanePhase = 0;

    pilotFrame = 0;
    pilotTimer = 0;
    planeFireFrame = 0;
    planeFireTimer = 0;

    // จับผู้เล่นไปยัดไว้ในเครื่องบินก่อน
    player.y = airplaneY;
    player.velocityY = 0;
}

void Game::Update() {
    // 1. ระบบพื้นฐาน
    if (IsKeyPressed(KEY_F)) {
        if (!IsWindowFullscreen()) {
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            ToggleFullscreen();
        } else {
            ToggleFullscreen();
            SetWindowSize(screenWidth, screenHeight);
        }
    }

    if (currentState == GameState::PLAYING && player.isAlive && !player.isWinner && !isPaused) {
        HideCursor();
    } else {
        ShowCursor();
    }

    // จัดการเสียงเพลงแยกไว้ต่างหาก
    UpdateAudio(); 

    // สับสวิตช์เลือกรันลอจิกตามหน้าจอ
    switch (currentState) {
        case GameState::SPLASH:   UpdateSplash(); break;
        case GameState::STORY:    UpdateStory(); break;
        case GameState::MENU:     UpdateMenu(); break;
        case GameState::CREDITS:  UpdateCredits(); break;
        case GameState::PLAYING:  UpdatePlaying(); break;
        case GameState::SURVIVAL: UpdateSurvival(); break;
        case GameState::VICTORY:  UpdateVictory(); break;
        case GameState::LOBBY:    UpdateLobby(); break;
    }
}

void Game::Draw() {
    BeginDrawing();
    
    // สับสวิตช์เลือกวาดภาพตามหน้าจอ
    switch (currentState) {
        case GameState::SPLASH:   DrawSplash(); break;
        case GameState::STORY:    DrawStory(); break;
        case GameState::MENU:     DrawMenu(); break;
        case GameState::CREDITS:  DrawCredits(); break;
        case GameState::PLAYING:  DrawPlaying(); break;
        case GameState::SURVIVAL: DrawSurvival(); break;
        case GameState::VICTORY:  DrawVictory(); break;
        case GameState::LOBBY:    DrawLobby(); break;
    }
    
    EndDrawing();
}

void Game::UpdateTimers() {
    if (player.iframeTimer > 0) player.iframeTimer--;

    if (cameraShakeTimer > 0) cameraShakeTimer--;
    if (screenFlashTimer > 0) screenFlashTimer--;
    
    if (player.shootCooldownTimer > 0) player.shootCooldownTimer--;
    if (player.outOfAmmoTimer > 0) player.outOfAmmoTimer--;

    // =================================================================
    // ระบบฟื้นฟูเลือดและแอนิเมชันหลอดเลือด
    // =================================================================
    static int healFrameCounter = 0; // ตัวนับเวลาสไตล์เฟรมเรต

    // ลอจิกการฮีลเลือดจริง HP ให้ค่อยๆ เพิ่มขึ้นไปหาเป้าหมาย targetHp
    if (player.hp < player.targetHp) {
        healFrameCounter++;
        if (healFrameCounter >= 100) {
            healFrameCounter = 0;
            player.hp++;
        }
    } else {
        healFrameCounter = 0; // ถ้าเลือดเต็มเป้าหมายแล้ว ให้รีเซ็ตตัวนับเวลา
    }

    // ลอจิกควบคุมหลอดเลือดสีเขียว displayHp ให้วิ่งไล่ตามเลือดจริง
    if (player.displayHp < (float)player.hp) {
        player.displayHp += 0.03f; // ความเร็วในการวิ่งขึ้นของหลอดเขียว
        if (player.displayHp > (float)player.hp) player.displayHp = (float)player.hp;
    } 
    else if (player.displayHp > (float)player.hp) {
        player.displayHp = (float)player.hp;
    }

    // =================================================================
    // อัปเดตและลบขยะ Floating Text
    // =================================================================
    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); ) {
        it->Update();
        if (it->lifeTimer <= 0) {
            it = floatingTexts.erase(it); // ลบออกจาก RAM เมื่อหมดเวลา
        } else {
            ++it;
        }
    }
}

void Game::UpdateBullets() {
    // เช็คลูกไฟศัตรูพุ่งชนผู้เล่น Of Flying Demon.
    Rectangle playerRect = { player.x, player.y, player.width, player.height };
    for (auto& b : bullets) {
        if (b.active && !b.isHit && b.isEnemyBullet) {
            Rectangle bulletRect = { b.x, b.y, b.width, b.height };
            
            if (CheckCollisionRecs(bulletRect, playerRect) && player.iframeTimer <= 0) {
                b.Hit(); // ลูกไฟระเบิดหายไป
                
                player.hp -= 2; // โดนลูกไฟเผา หักเลือด 2 หน่วย
                player.targetHp = player.hp;
                player.displayHp = player.hp;
                player.iframeTimer = 20; // ติดสถานะอมตะแป๊บนึง
                PlaySound(assets.playerHurtSound);
                
                screenFlashTimer = 10; // จอกระพริบสีแดง
                
                if (player.hp <= 0) {
                    player.isAlive = false;
                    player.deathCause = "INCINERATED BY FLYING DEMON"; // สาเหตุการตาย
                }
            }
        }
    }
    for (auto& b : bullets) {
        b.Update(); // ให้กระสุนจัดการตัวเอง
        if (std::abs(b.x - player.x) > 2000 || std::abs(b.y - player.y) > 2000) {
            b.active = false; // ถ้ายิงออกไปไกลมาก เคลียร์ทิ้ง
        }
    }

    // อัปเดตแอนิเมชันขยายวงระเบิด
    for (auto it = explosions.begin(); it != explosions.end(); ) {
        it->Update(); 
        if (!it->active) { 
            it = explosions.erase(it); // ลบออกจาก RAM เมื่อเล่นแอนิเมชันจบ
        } else {
            ++it;
        }
    }
    // อัปเดตและลบเอฟเฟคเลือด
    for (auto it = bloodEffects.begin(); it != bloodEffects.end(); ) {
        it->Update();
        int maxFrames = assets.bloodTex[it->type].size();
        
        if (it->currentFrame >= maxFrames) {
            it = bloodEffects.erase(it); 
        } else {
            ++it;
        }
    }
}

void Game::UpdateEnemies() {
    Rectangle playerRect = { player.x, player.y, player.width, player.height };
    if (CheckCollisionRecs(playerRect, goal)) {
        currentState = GameState::VICTORY; 
    }

    for (auto& e : enemies) {
        if (e.active) {
            // ให้ Host รัน AI ซอมบี้เท่านั้น ส่วน Client ให้รอรับพิกัดอย่างเดียว
            if (isHost) {
                Player* target = &player;
                if (player2.isAlive) {
                    // ถ้า Host ตายแล้ว หรือ Client ยืนอยู่ใกล้ซอมบี้มากกว่า ให้ล็อคเป้าไปที่ Client
                    if (!player.isAlive || std::abs(player2.x - e.x) < std::abs(player.x - e.x)) {
                        target = &player2;
                    }
                }
                e.Update(*target, platforms, gravity, items, assets);
            } else {
                if (e.state != ZombieState::DEAD) {
                    e.frameCounter++;
                    if (e.frameCounter >= 8) { 
                        e.frameCounter = 0; e.currentFrame++;
                        int maxF = (e.state == ZombieState::WALK) ? 10 : 6; 
                        if (e.currentFrame >= maxF) e.currentFrame = 0; 
                    }
                }
            }

            // =======================================================
            // [ระบบรับความเสียหาย]: ผู้เล่นแต่ละเครื่องเช็คโดนกัดด้วยตัวเอง
            // =======================================================
            if (e.state == ZombieState::ATTACK) {
                Rectangle enemyRect = { e.x, e.y, e.width, e.height };
                Rectangle myRect = { player.x, player.y, player.width, player.height };
                    
                // ถ้าซอมบี้กำลังกัด และสัมผัสโดนตัว
                if (CheckCollisionRecs(myRect, enemyRect) && player.iframeTimer <= 0) {
                    player.hp--; 
                    player.iframeTimer = 45; // ติดอมตะกระพริบแดงแป๊บนึง
                    PlaySound(assets.playerHurtSound);
                        
                    player.targetHp = player.hp; 
                    player.displayHp = player.hp;

                    if (player.hp <= 0) { 
                        player.isAlive = false; 
                        player.deathCause = "DEVOURED BY ZOMBIES"; 
                    }
                }
            }

            // ลอจิกกระสุนชนซอมบี้
            if (e.state != ZombieState::DEAD) {
                Rectangle enemyRect = { e.x, e.y, e.width, e.height };
                for (auto& b : bullets) {
                    if (b.active && !b.isHit && !b.isEnemyBullet) { 
                        Rectangle bulletRect = { b.x, b.y, b.width, b.height };
                        if (CheckCollisionRecs(bulletRect, enemyRect)) {
                            b.Hit();
                            if (b.type == 1) TriggerExplosion(b.x, b.y);

                            // เอฟเฟกต์เลือดให้ทำงานอิสระทั้ง 2 จอ
                            BloodEffect blood;
                            blood.x = e.x + (e.width / 2.0f); 
                            blood.y = e.y + (e.height / 2.0f);
                            blood.type = GetRandomValue(0, 4); 
                            blood.currentFrame = 0;
                            blood.frameCounter = 0;
                            blood.facingRight = (b.speedX > 0); 
                            bloodEffects.push_back(blood);

                            // ให้ Host เป็นคนหักเลือดและตัดสินความตายเท่านั้น
                            if (isHost) {
                                e.hp--; 
                                PlaySound(assets.hitSound); 

                                if (e.hp <= 0) player.score += 20;
                                
                                if (e.hp > 0) {
                                    e.state = ZombieState::HURT;
                                    e.stateTimer = 15; 
                                    e.currentFrame = 0;
                                }
                            } else {
                                PlaySound(assets.hitSound);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Game::UpdateBoss() {
    if (!boss.active && currentCutscene != CutsceneType::INTRO && currentCutscene != CutsceneType::PHASE2 && currentCutscene != CutsceneType::PHASE3_INTRO) return;

    // 1. Boss Physics & Gravity
    boss.velocityY += gravity;
    boss.y += boss.velocityY;
    for (const auto& box : platforms) {
        bool isInsideX = (boss.x + boss.width > box.x) && (boss.x < box.x + box.width);
        bool isFallingOnTop = (boss.y + boss.height >= box.y) && (boss.y + boss.height <= box.y + boss.velocityY + 5.0f) && (boss.velocityY >= 0);
        if (isInsideX && isFallingOnTop) { 
            boss.y = box.y - boss.height; 
            boss.velocityY = 0; 
        }
    }

    // Safety Trigger: Enrage mode หากบอสตกลงไปในหลุม
    if (boss.y > 750.0f && !boss.isEnraged && boss.state != BossState::DEAD) {
        boss.isEnraged = true;
        boss.velocityY = -25.0f; 
        PlaySound(assets.bossScream2Sound); 
        cameraShakeTimer = 60; 
    }

    // 2. Boss Animation Updates
    boss.frameCounter++;
    int animSpeed = 5; 
    
    if (boss.isEnraged) animSpeed = 3; 
    else if (boss.hp <= boss.maxHp / 2) {
        if (boss.state == BossState::CLEAVE) animSpeed = 5;  
        else if (boss.state == BossState::WALK) animSpeed = 8; 
        else if (boss.state == BossState::SUMMON) animSpeed = 10; 
        else animSpeed = 6;  
    }
    
    if (currentCutscene == CutsceneType::PHASE2) animSpeed = 12;

    if (boss.frameCounter >= animSpeed) {
        boss.frameCounter = 0;
        boss.currentFrame++;
    }

    // 3. Boss State Machine Updates
    if (boss.state == BossState::WALK) {
        if (boss.currentFrame >= assets.bossWalkTex.size()) boss.currentFrame = 0;
        if ((boss.currentFrame == 0 || boss.currentFrame == 6) && boss.frameCounter == 0) { 
            PlaySound(assets.bossWalkSound);
            if (boss.hp <= boss.maxHp / 2 || boss.isEnraged) cameraShakeTimer = 12; 
        }
    }
    else if (boss.state == BossState::TAKE_HIT) {
        float pushSpeed = (boss.hp <= boss.maxHp / 2) ? 0.2f : (boss.speed * 0.3f);
        float distX = player.x - boss.x;
        boss.x += (distX > 0) ? pushSpeed : -pushSpeed;

        if (boss.currentFrame >= assets.bossTakeHitTex.size()) {
            boss.state = BossState::WALK;
            boss.currentFrame = 0;
        }
    }
    else if (boss.state == BossState::CLEAVE) {
        if (boss.currentFrame == 8 && boss.frameCounter == 0) PlaySound(assets.bossCleaveSound);

        if (boss.currentFrame == 10 && boss.frameCounter == 0) {
            float reachFront = (boss.hp <= boss.maxHp / 2) ? 250.0f : 120.0f; 
            float reachBack  = (boss.hp <= boss.maxHp / 2) ? 100.0f : 50.0f;   
            
            if (boss.isEnraged) reachFront = 500.0f; 

            Rectangle bossAttackArea;
            if (boss.facingRight) bossAttackArea = { boss.x - reachBack, boss.y, reachBack + boss.width + reachFront, boss.height };
            else bossAttackArea = { boss.x - reachFront, boss.y, reachFront + boss.width + reachBack, boss.height };
            
            Rectangle playerRect = { player.x, player.y, player.width, player.height };
            
            // Damage handling
            if (CheckCollisionRecs(bossAttackArea, playerRect) && player.iframeTimer <= 0) {
                int damage = (boss.hp <= boss.maxHp / 2) ? 5 : 2;
                if (boss.isEnraged) damage = 10; 
                
                player.hp -= damage; 
                player.targetHp = player.hp;
                player.displayHp = player.hp;
                player.iframeTimer = 20; 
                PlaySound(assets.playerHurtSound); 
                
                cameraShakeTimer = 15; screenFlashTimer = 10; 
                player.velocityY = -6.0f; 
                if (player.x < boss.x) player.knockbackX = -18.0f; else player.knockbackX = 18.0f;                    

                if (player.hp <= 0) {
                player.isAlive = false;
                player.deathCause = "SLAIN BY DEMON KING";
                }
            }
        }
        if (boss.currentFrame >= assets.bossCleaveTex.size()) {
            boss.state = BossState::WALK;
            boss.currentFrame = 0;
            boss.attackCooldownTimer = (boss.isEnraged) ? 10 : 90; 
        }
    }
    else if (boss.state == BossState::SUMMON) {
        cameraShakeTimer = 5; 
        if (boss.currentFrame == 0 && boss.frameCounter == 1) PlaySound(assets.bossScream2Sound); 
        if (boss.currentFrame == 8 && boss.frameCounter == 1) {
            enemies.push_back(Enemy(boss.x - 400, 450, 2.8f));
            enemies.push_back(Enemy(boss.x + 400, 450, 2.8f));
        }
        if (boss.currentFrame >= 16) {
            boss.state = BossState::WALK; boss.currentFrame = 0; boss.stateTimer = 0; 
        }
    }
    else if (boss.state == BossState::DEAD) {
        if (!isPhase3Escape && currentCutscene != CutsceneType::PHASE3_INTRO) {
            // Cutscene Transition Trigger
            if (boss.currentFrame >= 8) {
                currentCutscene = CutsceneType::PHASE3_INTRO;
                cutsceneTimer = 360; 
                
                friendX = player.x + 250.0f; 
                friendY = -500; 
                friendScale = 1.0f;
                friendRedEye = false;
            }
        }
        else if (currentCutscene == CutsceneType::PHASE3_INTRO) {
            if (!friendRedEye) {
                // Loop animation indices 8 and 9
                if (boss.currentFrame > 9 || boss.currentFrame < 8) boss.currentFrame = 8;
                if (boss.frameCounter == 0) boss.currentFrame = (boss.currentFrame == 8) ? 9 : 8;
            } else {
                // Continue death animation
                if (boss.currentFrame >= assets.bossDeathTex.size() - 1) {
                    boss.currentFrame = assets.bossDeathTex.size() - 1; 
                }
            }
        }
        return; 
    }

    // 4. Boss AI Decision Making
    if (currentCutscene == CutsceneType::NONE && boss.state != BossState::CLEAVE && boss.state != BossState::TAKE_HIT && boss.state != BossState::DEAD && boss.state != BossState::SUMMON) {
        
        if (boss.attackCooldownTimer > 0) boss.attackCooldownTimer--;
        float distX = player.x - boss.x;
        boss.facingRight = (distX > 0); 

        float attackRange = (boss.hp <= boss.maxHp / 2) ? 240.0f : 100.0f;
        if (boss.isEnraged) attackRange = 400.0f; 

        if (abs(distX) <= attackRange && boss.attackCooldownTimer <= 0) {
            boss.state = BossState::CLEAVE; boss.currentFrame = 0; boss.frameCounter = 0;
        } 
        else {
            if (boss.isEnraged) {
                boss.state = BossState::WALK;
                float rushSpeed = 8.0f; 
                boss.x += (distX > 0) ? rushSpeed : -rushSpeed;
            }
            else if (boss.hp > boss.maxHp / 2) { 
                boss.stateTimer++; 
                if (boss.stateTimer < 180) {
                    boss.state = BossState::WALK; boss.x += (distX > 0) ? boss.speed : -boss.speed;
                }
                else if (boss.stateTimer >= 180 && boss.stateTimer < 240) boss.state = BossState::IDLE;
                else if (boss.stateTimer >= 240 && boss.stateTimer < 330) {
                    boss.state = BossState::WALK; float rushSpeed = boss.speed * 3.0f; boss.x += (distX > 0) ? rushSpeed : -rushSpeed;
                } else boss.stateTimer = 0;
            } 
            else {
                boss.stateTimer++; 
                if (boss.stateTimer >= 480) {
                    boss.state = BossState::SUMMON; boss.currentFrame = 0; boss.frameCounter = 0;
                } else {
                    boss.state = BossState::WALK; float currentSpeed = 0.7f; boss.x += (distX > 0) ? currentSpeed : -currentSpeed;
                }
            }
        }
    }

    // 5. Boss Collision with Bullets
    Rectangle bossRect = { boss.x, boss.y, boss.width, boss.height };
    for (auto& b : bullets) {
        if (b.active && !b.isHit && !b.isEnemyBullet) {
            Rectangle bulletRect = { b.x, b.y, b.width, b.height };
            if (CheckCollisionRecs(bulletRect, bossRect)) {
                b.Hit();
                if (b.type == 1) TriggerExplosion(b.x, b.y);
                
                if (boss.isEnraged) {
                    PlaySound(assets.crateHitSound); 
                    continue; 
                }

                // ==========================================
                bool isDashingPhase1 = (boss.hp > boss.maxHp / 2) && (boss.stateTimer >= 240 && boss.stateTimer < 330);

                if (boss.state == BossState::CLEAVE || boss.state == BossState::SUMMON || isDashingPhase1) {
                    boss.hp--;
                    PlaySound(assets.ironBodySound);
                } 
                else {
                    boss.hp--; 
                    PlaySound(assets.hitSound);
                
                    if (boss.hp > 0) {
                        boss.state = BossState::TAKE_HIT;
                        boss.currentFrame = 0;
                    }
                }
            // ==========================================

                if (boss.hp <= 0) {
                    player.score += 500;
                    boss.state = BossState::DEAD; 
                    boss.currentFrame = 0; 
                    boss.frameCounter = 0; 
                    PlaySound(assets.deathSound);

                    // Cinematic Shockwave Trigger
                    screenFlashTimer = 20; 
                    cameraShakeTimer = 30;
                    
                    player.velocityY = -15.0f; 
                    
                    if (player.x > boss.x) {
                        player.knockbackX = -50.0f; 
                    } else {
                        player.knockbackX = -20.0f; 
                    }
                }
            }
        }
    }

    // 6. Supply Drop System
    if (boss.active && boss.state != BossState::DEAD && !boss.isEnraged) {
        supplyDropTimer++;
        
        if (supplyDropTimer >= 420) {
            supplyDropTimer = 0; 
            
            float dropX = player.x + (GetRandomValue(0, 1) == 0 ? GetRandomValue(-300, -150) : GetRandomValue(150, 300));
            
            if (dropX < 6200) dropX = 6200;
            if (dropX > 9400) dropX = 9400;

            items.push_back(Item(dropX, 460, 1)); 
            
            PlaySound(assets.reloadSound);
        }
    }

    // ระบบกำจัดขยะ (Garbage Collector): ลบซอมบี้ที่ active = false ออกจาก RAM
    for (auto it = enemies.begin(); it != enemies.end(); ) {
        if (!it->active) {
            it = enemies.erase(it); // ลบออกจาก Vector ถาวร
        } else {
            ++it;
        }
    }
}

void Game::UpdateItems() {
    Rectangle playerRect = { player.x, player.y, player.width, player.height };

    for (auto& item : items) {
        if (item.active) {
            item.Update();
            // Lifetime Management
            if (item.type != 0) { // ถ้าไม่ใช่เหรียญ (ปืนพก/ยา ค่อยลดเวลา)
                item.lifeTimer--;
                if (item.lifeTimer <= 0) {
                    item.active = false; 
                    continue; 
                }
            }

            Rectangle itemRect = { item.x, item.y, item.width, item.height };
            if (CheckCollisionRecs(playerRect, itemRect)) {
                item.active = false;

                recentlyCollectedItems.push_back(item.id);
                
                if (item.type == 0) { // [0] สมบัติ (เหรียญ)
                    PlaySound(assets.coinPickupSound); 
                    player.score += 100;

                    // ระบบสร้างตัวหนังสือลอย
                    FloatingText ft;
                    ft.x = item.x;
                    ft.y = item.y - 20.0f; // ให้ลอยสูงกว่าเหรียญนิดนึง
                    ft.text = "+100";      // ข้อความที่จะโชว์
                    ft.color = GOLD;       // สี
                    ft.maxLife = 60;       // (60 เฟรม = 1 วินาที)
                    ft.lifeTimer = 60;
                    ft.speedY = 1.5f;      // ความเร็วในการลอยขึ้น
                    floatingTexts.push_back(ft);
                }
                else if (item.type == 1) { // [1] กล่องกระสุน
                    PlaySound(assets.ammoPickupSound); 
                    if (player.weaponType == 0) {
                        player.reserveAmmo[0] += 15; // ปืนพกได้ 15 นัด
                    } else if (player.weaponType == 1) {
                        player.reserveAmmo[1] += 2;  // RPG ได้ 2 นัด
                    }
                }
                else if (item.type == 2) { // [2] ยาพยาบาล
                    PlaySound(assets.foodPickupSound);
                    player.targetHp += 2;
                    if (player.targetHp > player.maxHp) player.targetHp = player.maxHp;

                    // โค้ดสร้างตัวหนังสือลอย
                    FloatingText ft;
                    ft.x = item.x;
                    ft.y = item.y - 20.0f; // ให้ลอยสูงกว่าเหรียญนิดนึง
                    ft.text = "+HP";      // ข้อความที่จะโชว์
                    ft.color = GREEN;       // สี
                    ft.maxLife = 60;       // (60 เฟรม = 1 วินาที)
                    ft.lifeTimer = 60;
                    ft.speedY = 1.5f;      // ความเร็วในการลอยขึ้น
                    floatingTexts.push_back(ft);
                }
            }
        }
    }
}

void Game::TriggerExplosion(float ex, float ey) {
    // 1. สร้างภาพแอนิเมชันระเบิด
    ExplosionEffect exp;
    exp.x = ex;
    exp.y = ey;
    exp.currentFrame = 0; 
    exp.frameCounter = 0;
    exp.active = true;
    explosions.push_back(exp);
    
    // 2. เอฟเฟกต์สะเทือน
    cameraShakeTimer = 20;
    PlaySound(assets.rpgExplosionSound);

    // 3. กวาดล้างศัตรูในรัศมี
    Vector2 blastCenter = { ex, ey };
    float blastRadius = 150.0f;

    // โดนซอมบี้
    for (auto& e : enemies) {
        if (e.active && e.state != ZombieState::DEAD) {
            Rectangle enemyRect = { e.x, e.y, e.width, e.height };
            if (CheckCollisionCircleRec(blastCenter, blastRadius, enemyRect)) {
                e.hp -= 4; // ดาเมจ RPG แรงกว่าปืนพก 4 เท่า
                if (e.hp <= 0) { e.state = ZombieState::DEAD; e.currentFrame = 0; PlaySound(assets.deathSound); }
                else { e.state = ZombieState::HURT; e.stateTimer = 15; e.currentFrame = 0; }
            }
        }
    }

    // โดนกล่องไม้
    for (auto& c : crates) {
        if (c.active && !c.isBroken) {
            Rectangle crateRect = { c.x, c.y, c.width, c.height };
            if (CheckCollisionCircleRec(blastCenter, blastRadius, crateRect)) {
                c.hp -= 4; 
                c.hitFlashTimer = 10; // กระพริบแดง

                if (c.hp <= 0 && !c.isBroken) {
                    c.isBroken = true;
                    c.brokenTimer = 60;
                    PlaySound(assets.crateBreakSound);
                    items.push_back(Item(c.x + 10, c.y + 20, GetRandomValue(0, 2))); // สุ่มของดรอป
                }
            }
        }
    }

    // โดนบอส
    if (boss.active && boss.state != BossState::DEAD) {
        Rectangle bossRect = { boss.x, boss.y, boss.width, boss.height };
        if (CheckCollisionCircleRec(blastCenter, blastRadius, bossRect)) {
            boss.hp -= 3; // หักเลือดบอส
            
            // เช็ค Super Armor
            bool isDashingPhase1 = (boss.hp > boss.maxHp / 2) && (boss.stateTimer >= 240 && boss.stateTimer < 330);
            if (boss.isEnraged || boss.state == BossState::CLEAVE || boss.state == BossState::SUMMON || isDashingPhase1) {
                PlaySound(assets.ironBodySound);
            } else if (boss.hp > 0) {
                boss.state = BossState::TAKE_HIT;
                boss.currentFrame = 0;
            }
        }
    }
}

void Game::UpdateCrates() {
    for (auto& c : crates) {
        if (c.active) {
            c.Update(); // อัปเดตแอนิเมชันทุกเฟรม

            if (c.isBroken) continue; // ถ้ากล่องพังไปแล้ว ให้กระสุนทะลุผ่านไป

            Rectangle crateRect = { c.x, c.y, c.width, c.height };

            for (auto& b : bullets) {
                if (b.active && !b.isHit && !b.isEnemyBullet) {
                    Rectangle bulletRect = { b.x, b.y, b.width, b.height };
                    
                    if (CheckCollisionRecs(bulletRect, crateRect)) {
                        b.Hit();
                        if (b.type == 1) TriggerExplosion(b.x, b.y);
                        
                        c.hp--;          
                        c.hitFlashTimer = 10; // สั่งให้กระพริบแดง 10 เฟรม

                        if (c.hp <= 0 && !c.isBroken) {
                            c.isBroken = true; 
                            c.brokenTimer = 60; // สั่งให้เล่นแอนิเมชันแตก 1 วินาที (60 เฟรม)
                            PlaySound(assets.crateBreakSound); 
                            PlaySound(assets.crateExpSound); 
                            
                            // สุ่มไอเทมกระเด็นออกมาทันทีที่แตก
                            int randType = 0; 
                            int chance = GetRandomValue(1, 100);
                            if (chance <= 60) randType = 0; 
                            else if (chance <= 90) randType = 1; 
                            else randType = 2; 
                            
                            items.push_back(Item(c.x + 10, c.y + 20, randType));
                        } else {
                            PlaySound(assets.crateHitSound); 
                        }
                    }
                }
            }
        }
    }
}

// =================================================================
// หมวด UPDATE: ลอจิกการคำนวณแต่ละหน้าจอ
// =================================================================
void Game::UpdateAudio() {
    // Main Menu / Credits BGM
    if (currentState == GameState::MENU || currentState == GameState::CREDITS) {
        if (!IsMusicStreamPlaying(assets.menuMusic)) PlayMusicStream(assets.menuMusic);
        UpdateMusicStream(assets.menuMusic);
    } else {
        if (IsMusicStreamPlaying(assets.menuMusic)) StopMusicStream(assets.menuMusic); 
    }

    // Victory Screen BGM
    if (currentState == GameState::VICTORY) {
        if (!IsMusicStreamPlaying(assets.victoryMusic)) PlayMusicStream(assets.victoryMusic);
        UpdateMusicStream(assets.victoryMusic);
    } else {
        if (IsMusicStreamPlaying(assets.victoryMusic)) StopMusicStream(assets.victoryMusic);
    }

    // Story Screen BGM
    if (currentState == GameState::STORY) {
        if (!IsMusicStreamPlaying(assets.storyMusic)) PlayMusicStream(assets.storyMusic);
        UpdateMusicStream(assets.storyMusic);
    } else {
        if (IsMusicStreamPlaying(assets.storyMusic)) StopMusicStream(assets.storyMusic);
    }

    // Gameplay BGM Logic (รวมโหมด Playing และ Survival)
    if (currentState == GameState::PLAYING || currentState == GameState::SURVIVAL) {
        if (!player.isAlive) {
            if (IsMusicStreamPlaying(assets.gameplayMusic)) StopMusicStream(assets.gameplayMusic);
            if (IsMusicStreamPlaying(assets.bossMusic))     StopMusicStream(assets.bossMusic);
            if (IsMusicStreamPlaying(assets.finalBossMusic)) StopMusicStream(assets.finalBossMusic);
        }
        else if (isPhase3Escape || currentCutscene == CutsceneType::PHASE3_INTRO || currentCutscene == CutsceneType::SURVIVAL_INTRO) {
            if (IsMusicStreamPlaying(assets.gameplayMusic)) StopMusicStream(assets.gameplayMusic);
            if (IsMusicStreamPlaying(assets.bossMusic)) StopMusicStream(assets.bossMusic);
            
            if (!IsMusicStreamPlaying(assets.finalBossMusic)) PlayMusicStream(assets.finalBossMusic);
            UpdateMusicStream(assets.finalBossMusic);
        }
        else if (boss.active || currentCutscene == CutsceneType::INTRO || currentCutscene == CutsceneType::PHASE2 || boss.state == BossState::DEAD) {
            if (IsMusicStreamPlaying(assets.gameplayMusic)) StopMusicStream(assets.gameplayMusic);
            if (IsMusicStreamPlaying(assets.finalBossMusic)) StopMusicStream(assets.finalBossMusic);
            
            if (!IsMusicStreamPlaying(assets.bossMusic)) PlayMusicStream(assets.bossMusic);
            UpdateMusicStream(assets.bossMusic);
        }
        else {
            if (IsMusicStreamPlaying(assets.bossMusic)) StopMusicStream(assets.bossMusic);
            if (IsMusicStreamPlaying(assets.finalBossMusic)) StopMusicStream(assets.finalBossMusic);
            
            if (!IsMusicStreamPlaying(assets.gameplayMusic)) PlayMusicStream(assets.gameplayMusic);
            UpdateMusicStream(assets.gameplayMusic);
        }
    } else {
        if (IsMusicStreamPlaying(assets.gameplayMusic)) StopMusicStream(assets.gameplayMusic);
        if (IsMusicStreamPlaying(assets.bossMusic)) StopMusicStream(assets.bossMusic);
        if (IsMusicStreamPlaying(assets.finalBossMusic)) StopMusicStream(assets.finalBossMusic);
    }
}

void Game::UpdateSplash() {
    splashTimer++;
    if (splashTimer > 420) { 
        currentState = GameState::MENU; 
        splashTimer = 0;
    }
}

void Game::UpdateStory() {
    Vector2 mousePos = GetMousePosition();
    Rectangle btnVideoTopRight = { (float)GetScreenWidth() - 250.0f, 40.0f, 200.0f, 50.0f };

    if (CheckCollisionPointRec(mousePos, btnVideoTopRight) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool wasFullscreen = IsWindowFullscreen(); 
        if (wasFullscreen) ToggleFullscreen(); 

        PauseMusicStream(assets.storyMusic);
        system("start /wait wmplayer \"%CD%\\assets\\intro.mp4\" /fullscreen /close");
        ResumeMusicStream(assets.storyMusic);
        
        if (wasFullscreen) ToggleFullscreen(); 
    }

    if (IsKeyReleased(KEY_ENTER)) {
        currentState = GameState::MENU;
    }
}

void Game::UpdateMenu() {
    if (IsKeyPressed(KEY_ENTER)) {
        Reset(); 
        currentState = GameState::PLAYING;
        PlaySound(assets.airplaneArriveSound);
    }
    
    Vector2 mousePos = GetMousePosition();
    
    // ขยับพิกัด Y ของทุกปุ่มให้ห่างกันช่องละ 60 พิกเซล
    Rectangle creditBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 20.0f, 200, 50 };
    if (CheckCollisionPointRec(mousePos, creditBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = GameState::CREDITS;
    }

    Rectangle storyBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 80.0f, 200, 50 };
    if (CheckCollisionPointRec(mousePos, storyBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = GameState::STORY; 
    }

    Rectangle survivalBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 140.0f, 200, 50 };
    if (CheckCollisionPointRec(mousePos, survivalBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Reset(); 
        currentState = GameState::SURVIVAL; 
        currentCutscene = CutsceneType::SURVIVAL_INTRO;
        cutsceneTimer = 360; 
        currentWave = 1;
        
        platforms.clear(); enemies.clear(); crates.clear(); items.clear();

        platforms.push_back({ -400, 600, 2080, 200 }); 
        platforms.push_back({ -400, -500, 100, 1500 }); 
        platforms.push_back({ 1680, -500, 100, 1500 }); 

        player.x = GetScreenWidth() / 2.0f - 300.0f;
        player.y = 500.0f;

        isPhase3Escape = true; 
        friendRedEye = false;
        friendX = GetScreenWidth() / 2.0f; 
        friendY = -300.0f; 
        friendScale = 2.0f; 

        currentWave = 0; 
        waveAnnounceTimer = 0; 
    }

    Rectangle lobbyBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 200.0f, 200, 50 };
    if (CheckCollisionPointRec(mousePos, lobbyBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = GameState::LOBBY; 
    }
}

void Game::UpdateCredits() {
    Vector2 mousePos = GetMousePosition();
    Rectangle backBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight() - 100.0f, 200, 50 };
    if ((CheckCollisionPointRec(mousePos, backBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ESCAPE)) {
        currentState = GameState::MENU;
    }
}

void Game::UpdatePlaying() {
    if (!player.isAlive && !gameOverSoundPlayed) {
        PlaySound(assets.gameOverSound);
        gameOverSoundPlayed = true;
    }
    
    // โค้ดปุ่ม Game Over
    if (!player.isAlive || player.isWinner) {
        if (IsKeyPressed(KEY_P)) currentState = GameState::MENU;
        if (!player.isAlive) {
            float scale = (float)GetScreenWidth() / 1280.0f;
            Rectangle btnRec = { (float)(GetScreenWidth() / 2 - (260 * scale) / 2), (float)(GetScreenHeight() / 2 + (90.0f * scale)), (float)(260 * scale), (float)(55 * scale) };
            if (CheckCollisionPointRec(GetMousePosition(), btnRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentState = GameState::MENU;
            }
        }
    }

    if (player.isAlive && !player.isWinner) {
        if (IsKeyPressed(KEY_O)) isPaused = !isPaused; 

        if (!isPaused) {
            float distToBoss = boss.x - player.x;

            // Cutscene Triggers
            if (distToBoss < 1200 && distToBoss > 600 && !boss.active && currentCutscene == CutsceneType::NONE) {
                currentCutscene = CutsceneType::WARNING;
                cutsceneTimer = 180;
                PlaySound(assets.warningSound);
            }
            else if (distToBoss <= 600 && !boss.active && currentCutscene != CutsceneType::INTRO) {
                StopSound(assets.warningSound);
                currentCutscene = CutsceneType::INTRO;
                cutsceneTimer = 150; 
                boss.state = BossState::IDLE;
                player.knockbackX = 0; 
            }
            else if (boss.hp <= boss.maxHp / 2 && !phase2Triggered && boss.active) {
                phase2Triggered = true;
                currentCutscene = CutsceneType::PHASE2;
                cutsceneTimer = 180; 
                boss.state = BossState::DEAD; 
                boss.currentFrame = 0;
                boss.frameCounter = 0;
                PlaySound(assets.bossScreamSound);

                flyingDemons.clear();
            }

            // Cutscene Updates
            if (currentCutscene == CutsceneType::AIRPLANE_DROP) {
                UpdateAirplaneIntro();
            }

            else if (currentCutscene == CutsceneType::INTRO) {
                cutsceneTimer--;
                if (cutsceneTimer <= 0) {
                    currentCutscene = CutsceneType::NONE;
                    boss.active = true; 
                    boss.state = BossState::WALK;
                    boss.currentFrame = 0;
                    boss.frameCounter = 0;

                    if (boss.hp > boss.maxHp / 2 && !phase2Triggered) {
                        flyingDemons.push_back(FlyingDemon(boss.x - 200.0f, boss.y - 150.0f));
                    }
                }
            }
            else if (currentCutscene == CutsceneType::PHASE2) {
                cutsceneTimer--;
                boss.frameCounter++;
                if (boss.frameCounter >= 12) { 
                    boss.frameCounter = 0;
                    if (boss.currentFrame < 11) boss.currentFrame++;
                }
                cameraShakeTimer = 3; 

                flyingDemons.clear(); 

                if (cutsceneTimer <= 0) {
                    currentCutscene = CutsceneType::NONE;
                    boss.state = BossState::WALK; 
                    boss.currentFrame = 0;
                    boss.frameCounter = 0;
                }
            }

            else if (currentCutscene == CutsceneType::PHASE3_INTRO) {
                UpdateBoss(); 
                cutsceneTimer--;
                
                if (friendY < boss.y - 200) friendY += 4.0f;
                camera.target.x += (friendX - camera.target.x) * 0.05f;
                camera.target.y += (friendY - camera.target.y) * 0.05f;

                if (cutsceneTimer == 180) {
                    friendRedEye = true;
                    PlaySound(assets.bossScream2Sound); 
                }
                if (friendRedEye) cameraShakeTimer = 5; 

                if (cutsceneTimer <= 0) {
                    currentCutscene = CutsceneType::NONE;
                    isPhase3Escape = true;
                    boss.active = false; 
                    
                    player.iframeTimer = 180; 
                    player.speed = 15.0f; 
                    phase3Delay = 120; 

                    platforms.clear(); enemies.clear(); crates.clear(); items.clear(); bullets.clear();

                    platforms.push_back({ player.x + 200, -1000, 100, 2000 }); 
                    float buildX = player.x + 100; 
                    while(buildX > 500) {
                        platforms.push_back({ buildX - 1000, 500, 1000, 200 }); 
                        buildX -= 1140; 
                    }
                    platforms.push_back({ -500, 500, 1000, 200 }); 
                    goal = { 0, 400, 100, 100 }; 

                    // ตั้งค่าเรียกเครื่องบินกู้ภัยทันที
                    escapePlaneState = 1;
                    escapePlaneX = player.x + 1500.0f; // มาจากด้านขวา

                    escapePlaneY = player.y - 120.0f; // ให้บินเฉียดหัวผู้เล่นเลย
                    
                    escapePlaneBaseY = goal.y - 120.0f; // ระดับความสูงตอนจอดรอที่เส้นชัย
                    escapePilotFrame = 0;
                    escapePilotTimer = 0;
                }
            }
            else {
                UpdateTimers();            
                player.UpdateMovement(platforms, gravity, jumpForce, crates);
                player.UpdateCombat(camera, bullets, assets);
                UpdateBullets();
                UpdateEnemies();            
                UpdateItems(); 
                UpdateCrates();
                UpdateBoss();

                // =================================================================
                // ระบบจัดการ Flying Demon
                // =================================================================
                
                // 1. สมอง AI ของปีศาจบินแต่ละตัว
                for (auto& fd : flyingDemons) {
                    fd.Update(player, bullets, assets);
                }

                // 2. ลอจิกเช็คกระสุนของผู้เล่น ยิงโดน Flying Demon
                for (auto& fd : flyingDemons) {
                    if (fd.state != DemonState::DEAD) {
                        Rectangle fdRect = { fd.x, fd.y, fd.width, fd.height };
                        
                        for (auto& b : bullets) {
                            // ต้องเป็นกระสุนของผู้เล่นเท่านั้น
                            if (b.active && !b.isHit && !b.isEnemyBullet) {
                                Rectangle bulletRect = { b.x, b.y, b.width, b.height };
                                
                                if (CheckCollisionRecs(bulletRect, fdRect)) {
                                    b.Hit(); // กระสุนระเบิด/แตกกระจาย
                                    if (b.type == 1) TriggerExplosion(b.x, b.y); // ถ้าเป็น RPG ให้ระเบิดวงกว้าง

                                    fd.hp--;
                                    PlaySound(assets.hitSound);

                                    if (fd.hp <= 0) {
                                        player.score += 150;
                                    } else {
                                        fd.state = DemonState::HURT;
                                        fd.stateTimer = 15;
                                        fd.currentFrame = 0;
                                    }
                                }
                            }
                        }
                    }
                }

                // 3. ระบบเก็บขยะ (Garbage Collector): ลบ active = false ออกจาก RAM
                for (auto it = flyingDemons.begin(); it != flyingDemons.end(); ) {
                    if (!it->active) {
                        it = flyingDemons.erase(it);
                    } else {
                        ++it;
                    }
                }
                // =================================================================

                if (isPhase3Escape) {
                    // =================================================================
                    // ระบบเครื่องบินมารับตอนหนีบอส (Phase 3)
                    // =================================================================
                    if (escapePlaneState > 0) {
                        // อัปเดตแอนิเมชันไฟท้ายเครื่องบิน
                        planeFireTimer++;
                        if (planeFireTimer >= 4) {
                            planeFireTimer = 0;
                            planeFireFrame++;
                            if (planeFireFrame >= 7) planeFireFrame = 0;
                        }

                        if (escapePlaneState == 1) {
                            escapePlaneX -= 30.0f; // บินสวนทางอย่างรวดเร็ว (จากขวาไปซ้าย)
                            if (escapePlaneX < goal.x - 300.0f) { // บินเลยเส้นชัยไปนิดนึงแล้ววนกลับมา
                                escapePlaneState = 2;
                                escapePlaneX = goal.x + 100.0f; // จอดรอที่เส้นชัย
                                escapePlaneY = escapePlaneBaseY;
                            }
                        } 
                        else if (escapePlaneState >= 2) {
                            // ลอยตัวรอ (Bobbing effect) ขึ้นลง
                            escapePlaneY = escapePlaneBaseY + (sinf(GetTime() * 8.0f) * 8.0f);
                        
                            if (escapePlaneState == 2) {
                                escapePilotFrame = 0;
                                // ถ้าผู้เล่นวิ่งเข้าใกล้เส้นชัย (ในระยะมองเห็น)
                                if (player.x < escapePlaneX + 900.0f) { 
                                    escapePlaneState = 3;
                                    escapePilotTimer = 0;
                                }
                            } 
                            else if (escapePlaneState == 3) {
                                // เล่นแอนิเมชันชูนิ้วโป้งต้อนรับ
                                escapePilotTimer++;
                                if (escapePilotTimer < 40) escapePilotFrame = 1; // เปลี่ยนเป็นหน้ายิ้ม
                                else escapePilotFrame = 2; // ชูนิ้วโป้งเยี่ยม!
                            }
                        }
                    }
                    // =================================================================
                        cameraShakeTimer = 2; 
                        if (phase3Delay > 0) phase3Delay--; 
                        else {
                            friendScale += 0.006f; 
                            friendX -= (player.speed * 1.15f); 
                        }

                        float fw = assets.friendTex.width * friendScale;
                        float fh = assets.friendTex.height * friendScale;
                        Rectangle friendRect = { friendX - fw/2.0f, friendY - fh/2.0f, fw, fh };
                        Rectangle playerRect = { player.x, player.y, player.width, player.height };
                    
                        if (CheckCollisionRecs(playerRect, friendRect) && player.iframeTimer <= 0) {
                            player.hp = 0; 
                            player.isAlive = false;
                            player.deathCause = "DEVOURED BY THE ABOMINATION";
                        }
                    }
                }  

                if (currentCutscene == CutsceneType::WARNING) {
                    cutsceneTimer--;
                    if (cutsceneTimer <= 0) {
                        currentCutscene = CutsceneType::NONE;
                        StopSound(assets.warningSound);
                    }
                }

                UpdateCamera();
            }
        }
    }


void Game::UpdateSurvival() {
    if (!player.isAlive && !gameOverSoundPlayed) {
        PlaySound(assets.gameOverSound);
        gameOverSoundPlayed = true;
    }

    if (!player.isAlive) {
        if (IsKeyPressed(KEY_P)) currentState = GameState::MENU;
        float scale = (float)GetScreenWidth() / 1280.0f;
        Rectangle btnRec = { GetScreenWidth() / 2.0f - 130.0f * scale, GetScreenHeight() / 2.0f + 90.0f * scale, 260.0f * scale, 55.0f * scale };
        if (CheckCollisionPointRec(GetMousePosition(), btnRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            currentState = GameState::MENU;
        }
    }

    if (player.isAlive && !isPaused) {
        if (currentCutscene == CutsceneType::SURVIVAL_INTRO) {
            cutsceneTimer--;
            if (friendY < 30.0f) friendY += 2.0f; 
            
            if (cutsceneTimer == 120) {
                cameraShakeTimer = 20;
                PlaySound(assets.bossScream2Sound);
                friendRedEye = true;
            }
            if (cutsceneTimer <= 0) currentCutscene = CutsceneType::NONE;
        }
        else {
            UpdateTimers();
            player.UpdateMovement(platforms, gravity, jumpForce, crates);
            player.UpdateCombat(camera, bullets, assets);
            UpdateBullets();
            UpdateItems();
            UpdateCrates();

            // เอฟเฟกต์บอสลอยกลับขึ้นฟ้าหลังจากคำรามเสร็จ
            if (friendY > -600.0f) {
                friendY -= 5.0f;
            }

            // ระบบ Wave
            if (waveAnnounceTimer > 0) waveAnnounceTimer--;

            int aliveZombies = 0;
            for (auto& e : enemies) {
                if (e.state != ZombieState::DEAD) aliveZombies++;
            }

            if (aliveZombies == 0 && enemiesToSpawn <= 0) {
                currentWave++; 
                if (currentWave > 1) {
                    crates.push_back(Crate(player.x - 150, 450));
                    crates.push_back(Crate(player.x + 150, 450));
                    PlaySound(assets.pickupSound);
                }
                
                enemiesToSpawn = currentWave * 4 + 2; 
                waveSpawnTimer = 180; 
                waveAnnounceTimer = 180; 

                PlaySound(assets.bossScream2Sound);
                cameraShakeTimer = 30;
            }

            if (enemiesToSpawn > 0) {
                waveSpawnTimer--;
                if (waveSpawnTimer <= 0) {
                    
                    // สั่งให้เฉพาะ Host เท่านั้นที่เสกซอมบี้เข้าฉากได้
                    if (isHost) {
                        float zombieSpeed = 1.0f + (currentWave * 0.25f);
                        if (zombieSpeed > 5.0f) zombieSpeed = 5.0f; 
                        int zombieHp = 3 + (currentWave / 2);

                        float dropX = GetRandomValue(800, 1150); 
                        float dropY = -200.0f;

                        Enemy newZombie(dropX, dropY, zombieSpeed);
                        newZombie.hp = zombieHp;
                        newZombie.maxHp = zombieHp;
                        
                        enemies.push_back(newZombie);
                    }
                    
                    enemiesToSpawn--;
                    waveSpawnTimer = GetRandomValue(20, 80); 
                }
            }
            UpdateEnemies(); 
        }

        // กล้องโหมด Survival
        if (currentCutscene == CutsceneType::SURVIVAL_INTRO) {
            camera.zoom += (0.65f - camera.zoom) * 0.05f; 
            camera.target.x += ((GetScreenWidth() / 2.0f) - camera.target.x) * 0.08f;
            camera.target.y += ((GetScreenHeight() / 2.0f) - camera.target.y) * 0.08f;
        } else {
            camera.zoom += (1.25f - camera.zoom) * 0.02f; 
            camera.target.x += (player.x - camera.target.x) * 0.04f;
            camera.target.y = (GetScreenHeight() / 2.0f) + 50.0f; 
            
            float visibleWidth = GetScreenWidth() / camera.zoom;
            float minX = -400.0f + (visibleWidth / 2.0f);
            float maxX = 1680.0f - (visibleWidth / 2.0f);
            
            if (camera.target.x < minX) camera.target.x = minX;
            if (camera.target.x > maxX) camera.target.x = maxX;
        }

        camera.offset = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };

        if (cameraShakeTimer > 0) {
            camera.offset.x += GetRandomValue(-15, 15);
            camera.offset.y += GetRandomValue(-15, 15);
        }
    }
}

void Game::UpdateVictory() {
    Vector2 mousePos = GetMousePosition();
    Rectangle returnBtn = { GetScreenWidth()/2.0f - 125, GetScreenHeight() - 120.0f, 250, 60 };
    
    if ((CheckCollisionPointRec(mousePos, returnBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_P)) {
        currentState = GameState::MENU;
    }
}

// =================================================================
// หมวด DRAW: การวาดภาพแต่ละหน้าจอ
// =================================================================

void Game::DrawSplash() {
    uiManager.DrawSplash(splashTimer, assets.splashLogoTex, assets.logoTex);
}

void Game::DrawStory() {
    uiManager.DrawStory(storyTex, assets.zombieIdleSheet);
}

void Game::DrawMenu() {
    ClearBackground(RAYWHITE);
    
    Rectangle bgSrc = { 0, 0, (float)assets.bgTex.width, (float)assets.bgTex.height };
    Rectangle bgDest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(assets.bgTex, bgSrc, bgDest, {0, 0}, 0.0f, WHITE);

    uiManager.DrawMenu();

    Vector2 mousePos = GetMousePosition();

    // วาดปุ่ม
    Rectangle storyBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 80.0f, 200, 50 };
    bool isHovering = CheckCollisionPointRec(mousePos, storyBtn);
    DrawRectangleRec(storyBtn, isHovering ? LIGHTGRAY : DARKGRAY);
    DrawRectangleLinesEx(storyBtn, 2, isHovering ? WHITE : GRAY);
    int textWidth = MeasureText("STORY", 20);
    DrawText("STORY", storyBtn.x + (storyBtn.width / 2) - (textWidth / 2), storyBtn.y + 15, 20, isHovering ? BLACK : WHITE);

    Rectangle survivalBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 140.0f, 200, 50 };
    bool isHoveringSurv = CheckCollisionPointRec(mousePos, survivalBtn);
    DrawRectangleRec(survivalBtn, isHoveringSurv ? MAROON : DARKGRAY);
    DrawRectangleLinesEx(survivalBtn, 2, isHoveringSurv ? RED : GRAY);
    int survTextWidth = MeasureText("SURVIVAL MODE", 20);
    DrawText("SURVIVAL MODE", survivalBtn.x + (survivalBtn.width / 2) - (survTextWidth / 2), survivalBtn.y + 15, 20, isHoveringSurv ? WHITE : LIGHTGRAY);

    Rectangle lobbyBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 200.0f, 200, 50 };
    bool isHoveringLobby = CheckCollisionPointRec(mousePos, lobbyBtn);
    DrawRectangleRec(lobbyBtn, isHoveringLobby ? DARKBLUE : DARKGRAY);
    DrawRectangleLinesEx(lobbyBtn, 2, isHoveringLobby ? BLUE : GRAY);
    int lobbyTextWidth = MeasureText("MULTIPLAYER", 20);
    DrawText("MULTIPLAYER", lobbyBtn.x + (lobbyBtn.width / 2) - (lobbyTextWidth / 2), lobbyBtn.y + 15, 20, isHoveringLobby ? WHITE : LIGHTGRAY);

    // =======================================================
    // 1. ป้าย BETA สติกเกอร์
    // =======================================================
    float time = GetTime();
    
    // สร้างจังหวะขยายขนาด (Breathing Effect) ด้วย Sine wave
    float betaScale = 1.0f + (sinf(time * 8.0f) * 0.15f); 
    
    const char* betaText = " BETA ";
    int betaFontSize = 18;
    float scaledFontSize = betaFontSize * betaScale;
    int betaTextWidth = MeasureText(betaText, betaFontSize); 
    
    // พิกัดมุมขวาบนของปุ่ม และองศาการเอียง
    Vector2 betaPos = { lobbyBtn.x + lobbyBtn.width - 5.0f, lobbyBtn.y + 5.0f };
    float rotation = -20.0f;
    
    // 1.1 วาดพื้นหลังป้ายสติกเกอร์สีแดง
    Rectangle stampRec = { betaPos.x, betaPos.y, (betaTextWidth + 12) * betaScale, (betaFontSize + 6) * betaScale };
    Vector2 stampOrigin = { stampRec.width / 2.0f, stampRec.height / 2.0f };
    DrawRectanglePro(stampRec, stampOrigin, rotation, RED);
    
    // 1.2 วาดเงาตัวหนังสือสีดำ
    Vector2 shadowPos = { betaPos.x + 2.0f, betaPos.y + 2.0f };
    Vector2 textOrigin = { (betaTextWidth * betaScale) / 2.0f, scaledFontSize / 2.0f };
    DrawTextPro(GetFontDefault(), betaText, shadowPos, textOrigin, rotation, scaledFontSize, 2, Fade(BLACK, 0.6f));
    
    // 1.3 วาดตัวหนังสือสีเหลือง
    DrawTextPro(GetFontDefault(), betaText, betaPos, textOrigin, rotation, scaledFontSize, 2, YELLOW);

    // =======================================================
    // 2. ข้อความเตือนใต้ปุ่ม
    // =======================================================
    const char* warningText = "[!] Work in Progress: Expect Bugs [!]";
    int warnWidth = MeasureText(warningText, 16);
    
    // สลับสีทุกๆ เสี้ยววินาที
    Color warnColor = ((int)(time * 6.0f) % 2 == 0) ? RED : ORANGE;
    
    // วาดข้อความเตือน
    DrawText(warningText, GetScreenWidth() / 2.0f - warnWidth / 2, lobbyBtn.y + 58, 16, warnColor);
}

void Game::DrawCredits() {
    ClearBackground(RAYWHITE);
    Rectangle bgSrc = { 0, 0, (float)assets.bgTex.width, (float)assets.bgTex.height };
    Rectangle bgDest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(assets.bgTex, bgSrc, bgDest, {0, 0}, 0.0f, WHITE);

    uiManager.DrawCredits(assets.profileTex, assets.logoTex);
}

void Game::DrawPlaying() {
    ClearBackground(RAYWHITE);

    Color bgTint = WHITE;
    if (currentCutscene == CutsceneType::PHASE3_INTRO && friendRedEye) bgTint = DARKGRAY; 
    if (isPhase3Escape) bgTint = MAROON; 
    
    Rectangle bgSrc = { 0, 0, (float)assets.bgTex.width, (float)assets.bgTex.height };
    Rectangle bgDest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(assets.bgTex, bgSrc, bgDest, {0, 0}, 0.0f, bgTint);

    // Camera Shake
    if (cameraShakeTimer > 0) {
        camera.offset.x = (GetScreenWidth() / 2.0f) + GetRandomValue(-15, 15);
        camera.offset.y = (GetScreenHeight() / 2.0f) + GetRandomValue(-15, 15);
    } else {
        camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    }

    BeginMode2D(camera);
        DrawWorld(); 
    EndMode2D();

    // Damage Flash
    if (screenFlashTimer > 0) {
        float alpha = (float)screenFlashTimer / 10.0f * 0.6f; 
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, alpha));
    }

    if (currentCutscene != CutsceneType::AIRPLANE_DROP) {
        uiManager.DrawHUD(player); 
    }

    if (currentCutscene == CutsceneType::WARNING) {
        if (cutsceneTimer % 20 < 10) { 
            DrawText("WARNING: DEMON KING APPROACHING!", GetScreenWidth()/2 - 380, GetScreenHeight()/2 - 150, 40, RED);
        }
    }

    // Boss HP Bar
    if (boss.active && boss.state != BossState::DEAD) {
        int barWidth = 600;
        int barHeight = 20;
        int barX = GetScreenWidth() / 2 - barWidth / 2;
        int barY = 80;

        if (boss.isEnraged) {
            int glitchX = barX + GetRandomValue(-10, 10);
            int glitchY = barY + GetRandomValue(-10, 10);
            DrawRectangle(glitchX, glitchY, barWidth, barHeight, BLACK);
            DrawRectangle(glitchX, glitchY, barWidth, barHeight, RED); 
            DrawText("FATAL ERROR: RUN FOR YOUR LIFE!!!", glitchX, glitchY - 25, 25, RED);
        } else {
            DrawRectangle(barX, barY, barWidth, barHeight, BLACK);
            float hpPercent = (float)boss.hp / (float)boss.maxHp;
            DrawRectangle(barX, barY, barWidth * hpPercent, barHeight, MAROON);
            DrawRectangleLines(barX, barY, barWidth, barHeight, GOLD); 
            DrawText("DEMON KING (RAID BOSS)", barX, barY - 25, 20, GOLD);
        }
    }
    // Phase 3 UI
    else if (isPhase3Escape) {
        int barWidth = 700; 
        int barHeight = 25;
        int barX = GetScreenWidth() / 2 - barWidth / 2;
        int barY = 80;

        int glitchX = barX + GetRandomValue(-10, 10);
        int glitchY = barY + GetRandomValue(-10, 10);
        
        DrawRectangle(glitchX, glitchY, barWidth, barHeight, BLACK);
        DrawRectangle(glitchX, glitchY, barWidth, barHeight, RED); 
        DrawRectangleLines(glitchX, glitchY, barWidth, barHeight, WHITE); 
        DrawText("God Not", glitchX + 3, glitchY - 27, 30, BLACK); 
        DrawText("God Not", glitchX, glitchY - 30, 30, RED); 

        const char* runText = "!!! RUN FOR YOUR LIFE !!!";
        int fontSize = 60;
        int textWidth = MeasureText(runText, fontSize);
        float rot = (float)GetRandomValue(-7, 7); 
        
        Vector2 textPos = { (float)(GetScreenWidth() / 2 + GetRandomValue(-15, 15)), 220.0f + GetRandomValue(-15, 15) };
        Vector2 origin = { textWidth / 2.0f, fontSize / 2.0f }; 
        Vector2 shadowPos = { textPos.x + 5, textPos.y + 5 };
        
        DrawTextPro(GetFontDefault(), runText, shadowPos, origin, rot, fontSize, 5, BLACK);
        DrawTextPro(GetFontDefault(), runText, textPos, origin, rot, fontSize, 5, RED);
    }

    if (isPaused) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
        const char* pauseText = "PAUSED";
        int textWidth = MeasureText(pauseText, 60);
        DrawText(pauseText, GetScreenWidth()/2 - textWidth/2, GetScreenHeight()/2 - 40, 60, WHITE);
        const char* resumeText = "Press [O] to Resume";
        int resumeWidth = MeasureText(resumeText, 20);
        DrawText(resumeText, GetScreenWidth()/2 - resumeWidth/2, GetScreenHeight()/2 + 40, 20, LIGHTGRAY);
    }
}

void Game::DrawSurvival() {
    ClearBackground(RAYWHITE);
    
    Color bgTint;
    if (currentCutscene == CutsceneType::SURVIVAL_INTRO && cutsceneTimer > 60) {
        bgTint = WHITE; 
    } else {
        bgTint = Color{ 220, 130, 80, 255 }; 
    }

    Rectangle bgSrc = { 0, 0, (float)assets.bgTex.width, (float)assets.bgTex.height };
    Rectangle bgDest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(assets.bgTex, bgSrc, bgDest, {0, 0}, 0.0f, bgTint);

    BeginMode2D(camera);
        for (const auto& pad : platforms) DrawRectangleRec(pad, DARKGRAY);
        for (auto& b : bullets) b.Draw(assets);
        for (const auto& blood : bloodEffects) blood.Draw(assets);
        for (const auto& exp : explosions) exp.Draw(assets);
        for (auto& item : items) item.Draw(assets);
        for (auto& e : enemies) e.Draw(assets);
        
        // 🛠️ Optimization: หากบอสลอยสูงพ้นจอไปแล้ว ให้ข้ามการวาดรูปบอสไปเลย (โค้ดที่เพิ่งแก้ไข)
        if (isPhase3Escape && friendY > -500.0f) {
            Texture2D tex = friendRedEye ? assets.friendRedEyeTex : assets.friendTex;
            float dWidth = tex.width * friendScale;
            float dHeight = tex.height * friendScale;
            Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
            Rectangle dest = { friendX, friendY, dWidth, dHeight };
            Vector2 origin = { dWidth / 2.0f, dHeight / 2.0f }; 
            
            float bossAlpha = 1.0f - ((friendScale - 2.0f) / 6.0f) * 0.99f; 
            if (bossAlpha < 0.01f) bossAlpha = 0.01f;
            if (bossAlpha > 1.0f) bossAlpha = 1.0f;

            DrawTexturePro(tex, src, dest, origin, 0.0f, Fade(WHITE, bossAlpha));
        }

        player.Draw(assets);
    EndMode2D();

    if (screenFlashTimer > 0) {
        float alpha = (float)screenFlashTimer / 10.0f * 0.6f; 
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, alpha));
    }

    uiManager.DrawHUD(player);

    if (currentCutscene == CutsceneType::NONE && player.isAlive) {
        const char* waveText = TextFormat("WAVE %d", currentWave);
        int textW = MeasureText(waveText, 40);
        DrawText(waveText, GetScreenWidth()/2 - textW/2, 80, 40, RED);

        int aliveCount = 0;
        for (const auto& e : enemies) {
            if (e.state != ZombieState::DEAD) aliveCount++;
        }
        int totalRemaining = aliveCount + enemiesToSpawn;

        const char* remainText = TextFormat("ZOMBIES REMAINING: %d", totalRemaining);
        int remW = MeasureText(remainText, 20);
        DrawText(remainText, GetScreenWidth()/2 - remW/2, 125, 20, WHITE); 

        if (waveAnnounceTimer > 0) {
            float alpha = (sinf(GetTime() * 10.0f) + 1.0f) / 2.0f; 
            const char* announceText = TextFormat("WAVE %d", currentWave);
            int annW = MeasureText(announceText, 80);
            DrawText(announceText, GetScreenWidth()/2 - annW/2, GetScreenHeight()/2 - 120, 80, Fade(RED, alpha));
            
            const char* subText = "PREPARE TO FIGHT!";
            int subW = MeasureText(subText, 30);
            DrawText(subText, GetScreenWidth()/2 - subW/2, GetScreenHeight()/2 - 30, 30, Fade(WHITE, alpha));
        }
    }

    if (currentCutscene == CutsceneType::SURVIVAL_INTRO) {
        if (cutsceneTimer > 60) {
            DrawText("THE ARENA OF GOD", GetScreenWidth()/2 - MeasureText("THE ARENA OF GOD", 60)/2, GetScreenHeight()/2 - 100, 60, GOLD);
            DrawText("SURVIVE THE HORDE!", GetScreenWidth()/2 - MeasureText("SURVIVE THE HORDE!", 30)/2, GetScreenHeight()/2 - 20, 30, WHITE);
        } else {
            DrawText("LET THE SLAUGHTER BEGIN!!!", GetScreenWidth()/2 - MeasureText("LET THE SLAUGHTER BEGIN!!!", 70)/2, GetScreenHeight()/2 - 50, 70, RED);
        }
    }
}

void Game::DrawVictory() {
    ClearBackground(RAYWHITE);
    uiManager.DrawVictory(player);
}

void Game::UpdateAirplaneIntro() {
    // เอฟเฟคไฟท้ายเครื่องบิน
    planeFireTimer++;
    if (planeFireTimer >= 4) {
        planeFireTimer = 0;
        planeFireFrame++;
        if (planeFireFrame >= 7) planeFireFrame = 0;
    }

    // =================================================================
    // ระบบผู้เล่น คำนวณแยกอิสระ ร่วงพื้นและกินน้ำตามเวลาของตัวเอง
    // =================================================================
    if (airplanePhase > 0) { // เริ่มคำนวณหลังจากเครื่องบินปล่อยตัวแล้ว
        // ลอจิกฟิสิกส์ตอนร่วงหล่นสู่พื้น
        if (!player.isGrounded && !player.isDrinking) {
            player.velocityY += gravity;
            player.y += player.velocityY;
            
            for (const auto& box : platforms) {
                float footCheckWidth = player.width * 0.6f;
                float footCheckX = player.x + (player.width * 0.2f);
                bool isInsideX = (footCheckX + footCheckWidth > box.x) && (footCheckX < box.x + box.width);
                
                if (isInsideX && (player.y + player.height >= box.y) && player.velocityY >= 0) {
                    player.y = box.y - player.height;
                    player.velocityY = 0;
                    player.isGrounded = true;
                    
                    // แตะพื้นปุ๊บ เปิดโหมดกินน้ำทันที
                    player.isDrinking = true;
                    player.currentDrinkFrame = 0;
                    player.drinkFrameCounter = 0;
                    break;
                }
            }
        }
    }

    // =================================================================
    // ลอจิกควบคุมแอนิเมชันกินน้ำ 13 ภาพ 
    // =================================================================
    if (player.isDrinking) {
        player.drinkFrameCounter++;
        
        // ระบบหน่วงเวลาเฟรม
        int delayTarget = 5; // ความเร็วภาพปกติ
        
        if (player.currentDrinkFrame == 7) {
            delayTarget = 60; // ภาพที่ 8 ค้างนาน 1 วินาที (60 เฟรม)
        } 
        else if (player.currentDrinkFrame == 12) {
            delayTarget = 30; // ภาพที่ 13 ค้างไว้สักพักก่อนสลับเป็นตัวปกติ
        }
        
        if (player.drinkFrameCounter >= delayTarget) {
            player.drinkFrameCounter = 0;
            player.currentDrinkFrame++;
            
            if (player.currentDrinkFrame >= 13) {
                player.isDrinking = false; // กินน้ำเสร็จสิ้น กลับสู่ร่างปกติ
            }
        }
    }

    // =================================================================
    // [ระบบเครื่องบิน] ควครองจังหวะตัวเอง
    // =================================================================
    if (airplanePhase == 0) {
        // [Phase 0: บินลงมาส่ง]
        airplaneY += 5.0f;
        player.y = airplaneY + 20.0f;
        player.x = airplaneX + 40.0f; 

        if (airplaneY >= 150.0f) {
            pilotTimer = 0;
            airplanePhase = 2;
        }
    }
    else if (airplanePhase == 2) {
        // จำลองแรงยกและแรงลม (ร่อนไปข้างหน้าช้าๆ + สั่นขึ้นลง)
        airplaneX += 0.5f; // ปรับความเร็วเครื่องบินร่อนไปข้างหน้า
        
        // ใช้สมการ Sine Wave สร้างจังหวะขึ้นลง
        airplaneY += sinf(GetTime() * 12.0f) * 0.6f; 

        pilotTimer++;
        if (pilotTimer == 20) pilotFrame = 1; 
        else if (pilotTimer == 40) pilotFrame = 2; // ชูนิ้วโป้งยิ้ม
        else if (pilotTimer == 120) { 
            pilotFrame = 0;
            airplanePhase = 3;

            // สั่งเล่นเสียงเร่งเครื่องบิน
            PlaySound(assets.airplaneDepartSound);
        }
    }

    else if (airplanePhase == 3) {
        // [Phase 3: บินเฉียงเชิดหัวออกนอกจอ]
        airplaneX += 10.0f;
        airplaneY -= 6.0f;

        if (airplaneY < -500.0f) { 
            currentCutscene = CutsceneType::NONE; // เครื่องบินลับตาไป จบคัตซีน คืนอิสระให้คนเล่น
        }
    }
}

void Game::UpdateCamera() {
    camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    
    // Cutscene Camera Modes
    if (currentCutscene == CutsceneType::INTRO || currentCutscene == CutsceneType::PHASE2) {
        
        float targetZoom = (boss.hp <= boss.maxHp / 2) ? 2.0f : 2.6f; 
        camera.zoom += (targetZoom - camera.zoom) * 0.05f; 
        
        float targetX = boss.x + boss.width / 2.0f;
        float feetY = boss.y + boss.height; 
        float targetY = feetY - 120.0f; 

        if (boss.hp <= boss.maxHp / 2) {
            targetY = feetY - 220.0f; 
        }
        
        camera.target.x += (targetX - camera.target.x) * 0.08f;
        camera.target.y += (targetY - camera.target.y) * 0.08f;
    } 
    // Normal Gameplay Camera Mode
    else {
        camera.zoom += (1.5f - camera.zoom) * 0.05f; 
        
        if (currentCutscene == CutsceneType::AIRPLANE_DROP) {
            // ล็อคแกน Y ไว้ที่พื้น (สมมติว่าพื้นคือประมาณ 400-500) 
            camera.target.y += (400.0f - camera.target.y) * 0.1f; 
        } else {
            // โค้ดเดิม: กล้องวิ่งตามแกน Y ของผู้เล่น
            camera.target.y += (player.y - camera.target.y) * 0.1f;
        }

        float visibleWidth = GetScreenWidth() / camera.zoom;
        float edgeMargin = visibleWidth * 0.35f; 
        float leftThreshold = camera.target.x - (visibleWidth / 2.0f) + edgeMargin;
        float rightThreshold = camera.target.x + (visibleWidth / 2.0f) - edgeMargin;

        if (player.x > rightThreshold) camera.target.x = player.x - (visibleWidth / 2.0f) + edgeMargin;
        else if (player.x < leftThreshold) camera.target.x = player.x + (visibleWidth / 2.0f) - edgeMargin;

        if (camera.target.x < visibleWidth / 2.0f) camera.target.x = visibleWidth / 2.0f;
    }
}

// -----------------------------------------------------------------
// Render Game World
// -----------------------------------------------------------------
void Game::DrawWorld() {
    // 1. วาดพื้น Platforms
    for (const auto& pad : platforms) {
        DrawRectangleRec(pad, GRAY);
    }

    // 2. วาดโครงสร้างเสาฐานราก
    for (float stripesX = 3730.0f; stripesX < 9600.0f; stripesX += 400.0f) {
        DrawRectangle(stripesX, 500, 60, 200, DARKGRAY);
    }

    // วาด Projectiles
    for (auto& b : bullets) {
        b.Draw(assets);
    }
    
    // วาดศัตรู (Enemies)
    for (auto& e : enemies) {
        e.Draw(assets);
    }

    // วาด Demon Flying
    for (auto& fd : flyingDemons) {
        fd.Draw(assets);
    }

    // วาดเลือด
    for (const auto& blood : bloodEffects) {
        blood.Draw(assets);
    }

    // วาดวงระเบิด (AoE Explosion)
    for (const auto& exp : explosions) {
        exp.Draw(assets);
    }

    // วาดไอเทมบนพื้น (Dropped Items)
    for (auto& item : items) {
        item.Draw(assets);
    }

    for (const auto& ft : floatingTexts) {
        ft.Draw();
    }

    // ---------------------------------------------------------
    // วาดบอส (Boss Render)
    // ---------------------------------------------------------
    if (!isPhase3Escape && (boss.active || currentCutscene == CutsceneType::INTRO || currentCutscene == CutsceneType::PHASE2 || boss.state == BossState::DEAD)) {
        // 🟢 เปลี่ยนมาเรียกฟังก์ชันนี้บรรทัดเดียว พร้อมส่งสถานะคัตซีนเข้าไปครับ
        boss.Draw(assets, currentCutscene == CutsceneType::PHASE2, cutsceneTimer);
    }

    // วาดจุดสิ้นสุดด่าน (เมื่อบอสตาย)
    if (boss.state == BossState::DEAD) {
        DrawRectangleRec(goal, Fade(LIME, 0.5f));
    }

    // วาดกล่องไม้ (Crates)
    for (auto& c : crates) {
        c.Draw(assets);
    }

    // ใน Game::DrawWorld()
    if (currentCutscene == CutsceneType::AIRPLANE_DROP) {
    // 1. วาดเอฟเฟกต์ไฟ
    DrawTextureEx(assets.planeFireTex[planeFireFrame], {airplaneX + 35, airplaneY + 30}, 0.0f, 0.8f, WHITE);

    // 2. วาดตัวเครื่องบิน
    DrawTexture(assets.airplaneTex, airplaneX, airplaneY, WHITE);

    // 3. วาดคนขับ
    DrawTexture(assets.pilotTex[pilotFrame], airplaneX + 75, airplaneY - 10, WHITE);
    }

    // ---------------------------------------------------------
    // วาดเครื่องบินกู้ภัย (Phase 3 Escape)
    // ---------------------------------------------------------
    if (isPhase3Escape && escapePlaneState > 0) {
        Texture2D planeTex = assets.airplaneTex;
        Texture2D fireTex = assets.planeFireTex[planeFireFrame];
        Texture2D pilotTex = assets.pilotTex[escapePilotFrame];

        if (escapePlaneState == 1) { 
            // Phase บินสวนมา: บินถอยหลังหันหน้ามาทางซ้าย (Flip)
            DrawTexturePro(planeTex, { 0, 0, -(float)planeTex.width, (float)planeTex.height }, 
                           { escapePlaneX, escapePlaneY, (float)planeTex.width, (float)planeTex.height }, 
                           {0, 0}, 0.0f, WHITE);
            
            float pilotOffsetNewX = (float)planeTex.width - 75.0f - (float)pilotTex.width;
            DrawTexturePro(pilotTex, { 0, 0, -(float)pilotTex.width, (float)pilotTex.height }, 
                           { escapePlaneX + pilotOffsetNewX, escapePlaneY - 10.0f, (float)pilotTex.width, (float)pilotTex.height }, 
                           {0, 0}, 0.0f, WHITE);

            float fireOffsetNewX = (float)planeTex.width - 35.0f - (float)fireTex.width;
            DrawTexturePro(fireTex, { 0, 0, -(float)fireTex.width, (float)fireTex.height }, 
                           { escapePlaneX + fireOffsetNewX, escapePlaneY + 30.0f, (float)fireTex.width, (float)fireTex.height }, 
                           {0, 0}, 0.0f, WHITE);
        } 
        else { 
            // 🎯 Phase จอดรอที่เส้นชัย (จอดหันหน้าไปทางซ้าย เตรียมหนี!)
            // Flip ทั้งเครื่องบิน คนขับ และไฟ เพื่อให้หันซ้ายทั้งหมด
            DrawTexturePro(planeTex, { 0, 0, -(float)planeTex.width, (float)planeTex.height }, 
                           { escapePlaneX, escapePlaneY, (float)planeTex.width, (float)planeTex.height }, 
                           {0, 0}, 0.0f, WHITE);
            
            float pilotOffsetNewX = (float)planeTex.width - 75.0f - (float)pilotTex.width;
            DrawTexturePro(pilotTex, { 0, 0, -(float)pilotTex.width, (float)pilotTex.height }, 
                           { escapePlaneX + pilotOffsetNewX, escapePlaneY - 10.0f, (float)pilotTex.width, (float)pilotTex.height }, 
                           {0, 0}, 0.0f, WHITE);
            
            float fireOffsetNewX = (float)planeTex.width - 35.0f - (float)fireTex.width;
            DrawTexturePro(fireTex, { 0, 0, -(float)fireTex.width, (float)fireTex.height }, 
                           { escapePlaneX + fireOffsetNewX, escapePlaneY + 30.0f, (float)fireTex.width, (float)fireTex.height }, 
                           {0, 0}, 0.0f, WHITE);
        }
    }

    // วาดผู้เล่น (Player Render)
    player.Draw(assets);

    if (player2.isAlive) {
        player2.Draw(assets); 
    }

    // ---------------------------------------------------------
    // Phase 3 (Joke Boss) Render
    // ---------------------------------------------------------
    if (currentCutscene == CutsceneType::PHASE3_INTRO || isPhase3Escape) {
        Texture2D tex = friendRedEye ? assets.friendRedEyeTex : assets.friendTex;
        
        float dWidth = tex.width * friendScale;
        float dHeight = tex.height * friendScale;
        Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
        Rectangle dest = { friendX, friendY, dWidth, dHeight };
        Vector2 origin = { dWidth / 2.0f, dHeight / 2.0f }; 
        
        // Shake Effect 
        if (isPhase3Escape) {
            dest.x += GetRandomValue(-10, 10);
            dest.y += GetRandomValue(-10, 10);
        }
        
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    }
}

bool Game::ShouldClose() const {
    return WindowShouldClose();
}

void Game::Close() {
    if (storyTex.id > 0) UnloadTexture(storyTex);

    assets.UnloadAll(); 
    CloseAudioDevice();
    CloseWindow();
}

void Game::UpdateLobby() {
    Vector2 mousePos = GetMousePosition();
    // อัปเดตพิกัด Hitbox
    Rectangle hostBtn = { GetScreenWidth()/2.0f - 180, GetScreenHeight()/2.0f - 70, 360, 50 };
    Rectangle joinBtn = { GetScreenWidth()/2.0f - 180, GetScreenHeight()/2.0f, 360, 50 };
    Rectangle ipBox   = { GetScreenWidth()/2.0f - 40, GetScreenHeight()/2.0f + 75, 200, 40 };
    Rectangle backBtn = { GetScreenWidth()/2.0f - 100, GetScreenHeight()/2.0f + 160, 200, 50 };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, ipBox)) typingIp = true;
        else typingIp = false;

        if (CheckCollisionPointRec(mousePos, hostBtn)) {
            requestHost = true;
            Reset();
            currentState = GameState::PLAYING;
            PlaySound(assets.airplaneArriveSound);
        }
        if (CheckCollisionPointRec(mousePos, joinBtn)) {
            requestJoin = true;
            Reset();
            currentState = GameState::PLAYING;
            PlaySound(assets.airplaneArriveSound);
        }
        if (CheckCollisionPointRec(mousePos, backBtn)) {
            currentState = GameState::MENU;
        }
    }

    if (typingIp) {
        int key = GetCharPressed();
        while (key > 0) {
            if (((key >= 48 && key <= 57) || key == 46) && joinIp.length() < 15) {
                joinIp += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && joinIp.length() > 0) joinIp.pop_back();
    }
}

void Game::DrawLobby() {
    ClearBackground(RAYWHITE);
    Rectangle bgSrc = { 0, 0, (float)assets.bgTex.width, (float)assets.bgTex.height };
    Rectangle bgDest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(assets.bgTex, bgSrc, bgDest, {0, 0}, 0.0f, Fade(WHITE, 0.5f)); // พื้นหลังจางๆ

    uiManager.DrawLobby(joinIp, typingIp);
}