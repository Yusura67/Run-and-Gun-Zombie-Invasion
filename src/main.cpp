// main.cpp
#include "Game.h"
#include "NetworkManager.h"
#include <cstdlib>
#include <cmath>

int main() {
    NetworkManager net;
    if (!net.Initialize()) return 1;

    Game game;

    while (!game.ShouldClose()) {
        
        // เช็คว่าผู้เล่นกดปุ่ม Host หรือ Join มาจากหน้า Lobby หรือไม่
        if (game.requestHost) { 
            net.CreateServer(7777, 4); 
            game.isHost = true; 
            game.requestHost = false; // รันเสร็จสับสวิตช์ปิด
        }
        if (game.requestJoin) { 
            // IP ที่ผู้เล่นพิมพ์ไว้ในช่องมาใช้เชื่อมต่อ
            net.JoinServer(game.joinIp, 7777); 
            game.isHost = false; 
            game.requestJoin = false; // รันเสร็จสับสวิตช์ปิด

            game.enemies.clear();
            game.items.clear();
        }
        
        net.Update(); 

        // 1. ถ่ายรูปนับจำนวนสิ่งของและศัตรูในฉากไว้ก่อน
        size_t oldItemCount = game.items.size();
        size_t oldEnemyCount = game.enemies.size();

        game.Update(); 

        // =================================================================
        // 2. ศูนย์กลางควบคุมสภาวะโลกคู่ขนาน (Multiplayer Sync)
        // =================================================================
        if (net.hasOtherPlayer) {
            net.SendPlayerSync(
                game.player.x, game.player.y, game.player.facingRight,
                game.player.rotation, game.player.weaponType,
                game.player.isMoving, game.player.isGrounded, 
                game.player.currentLegFrame, game.player.isAlive,
                game.player.justShot
            );

            // ------------------------------------------------------------------
            // ระบบซิงค์ไอเทม + ระบบซิงค์การเกิดใหม่ของซอมบี้
            // ------------------------------------------------------------------
            if (game.isHost) {
                // Host ส่งรหัสไอเทมใหม่
                for (size_t i = oldItemCount; i < game.items.size(); i++) {
                    net.SendItemSpawn(game.items[i].x, game.items[i].y, game.items[i].type, game.items[i].id);
                }
                // Host ส่งพิกัดและสมองซอมบี้ทุกตัวบนจอ ไปบังคับให้ Client วาดตาม
                for (auto& z : game.enemies) {
                    net.SendEnemySync(z.id, z.x, z.y, z.facingRight, z.active, z.hp, (int)z.state, z.currentFrame);
                }
            } 
            else {
                // [ฝั่ง CLIENT]: หั่นไอเทมที่แอบงอกเองทิ้ง
                if (game.items.size() > oldItemCount) game.items.erase(game.items.begin() + oldItemCount, game.items.end());
                
                // 🧟 [ฝั่ง CLIENT]: หั่นซอมบี้ผีหลอก (ที่ระบบ Survival แอบสุ่มเกิด) ทิ้ง
                if (game.enemies.size() > oldEnemyCount) game.enemies.erase(game.enemies.begin() + oldEnemyCount, game.enemies.end());
                
                // 🧟 [ฝั่ง CLIENT]: ฟังคำสั่ง Host จับซอมบี้วาร์ปให้ตรงกัน
                for (const auto& netZ : net.enemiesToSync) {
                    bool found = false;
                    for (auto& z : game.enemies) {
                        if (z.id == netZ.id) { // ถ้าตัวไหน ID ตรง จับวาร์ปและก็อปปี้แอนิเมชัน
                            z.x = netZ.x;
                            z.y = netZ.y;
                            z.facingRight = netZ.facingRight;
                            z.active = netZ.active;
                            z.hp = netZ.hp;
                            z.state = (ZombieState)netZ.state;
                            z.currentFrame = netZ.currentFrame;
                            found = true;
                            break;
                        }
                    }
                    // ถ้าซอมบี้ตัวนี้เพิ่งเกิดใหม่บน Host (หาบน Client ไม่เจอ) ให้สร้างโคลนขึ้นมาทันที
                    if (!found && netZ.active) {
                        Enemy newZ(netZ.x, netZ.y, 2.0f); 
                        newZ.id = netZ.id; // ยัด ID ให้ตรงกับ Host
                        newZ.facingRight = netZ.facingRight;
                        newZ.hp = netZ.hp;
                        newZ.maxHp = netZ.hp; // เซ็ตเลือดหลอดเต็มไว้ตามที่ส่งมา
                        newZ.state = (ZombieState)netZ.state;
                        newZ.currentFrame = netZ.currentFrame;
                        game.enemies.push_back(newZ);
                    }
                }
                net.enemiesToSync.clear(); // วาร์ปเสร็จเคลียร์ตะกร้าทิ้ง
            }

            // ------------------------------------------------------------------
            // โค้ดรับไอเทม
            // ------------------------------------------------------------------
            for (const auto& itemData : net.itemsToSpawn) {
                Item netItem(itemData.x, itemData.y, itemData.type);
                netItem.id = itemData.id; 
                game.items.push_back(netItem);
            }
            net.itemsToSpawn.clear();

            for (int itemId : game.recentlyCollectedItems) {
                net.SendItemDestroyed(itemId);
            }
            game.recentlyCollectedItems.clear(); 

            for (int destroyId : net.itemsToDestroy) {
                for (auto& item : game.items) {
                    if (item.id == destroyId && item.active) { 
                        item.active = false; 

                        Game::FloatingText ft;
                        ft.maxLife = 60; ft.lifeTimer = 60; ft.speedY = 1.5f;
                        ft.x = item.x; ft.y = item.y - 20.0f;

                        if (item.type == 0) { 
                            PlaySound(game.assets.coinPickupSound);
                            ft.text = "+100"; ft.color = GOLD; ft.Update(); game.floatingTexts.push_back(ft);
                        } else if (item.type == 2) { 
                            PlaySound(game.assets.foodPickupSound);
                            ft.text = "+HP"; ft.color = GREEN; ft.Update(); game.floatingTexts.push_back(ft);
                        }
                    }
                }
            }
            net.itemsToDestroy.clear(); 

            // ------------------------------------------------------------------
            // ซิงค์ร่างเพื่อน
            // ------------------------------------------------------------------
            game.player2.isAlive = net.otherPlayer.isAlive;
            game.player2.x = net.otherPlayer.x;
            game.player2.y = net.otherPlayer.y; 
            game.player2.facingRight = net.otherPlayer.facingRight;
            game.player2.isLegFacingRight = net.otherPlayer.facingRight;
            game.player2.rotation = net.otherPlayer.rotation;
            game.player2.weaponType = net.otherPlayer.weaponType;
            game.player2.isMoving = net.otherPlayer.isMoving; 
            game.player2.isGrounded = net.otherPlayer.isGrounded;
            game.player2.currentLegFrame = net.otherPlayer.currentLegFrame;

            if (game.player2.recoilAngle > 0.0f) {
                game.player2.recoilAngle -= 2.5f; 
                if (game.player2.recoilAngle < 0.0f) game.player2.recoilAngle = 0.0f;
            }

            if (net.otherPlayer.justShot && game.player2.isAlive) {
                float rad = game.player2.rotation * (PI / 180.0f);
                float aimDx = cos(rad); float aimDy = sin(rad);
                float startX = game.player2.x + (game.player2.width / 2);
                float startY = game.player2.y + (game.player2.height / 2);

                if (game.player2.weaponType == 1) { 
                    game.bullets.push_back(Bullet(startX - 10, startY - 10, aimDx * 8.0f, aimDy * 8.0f, 1));
                    game.player2.recoilAngle = 60.0f;
                    PlaySound(game.assets.rpgShootSound); 
                } else { 
                    game.bullets.push_back(Bullet(startX - 5, startY - 5, aimDx * 15.0f, aimDy * 15.0f, 0));
                    game.player2.recoilAngle = 15.0f; 
                    game.assets.PlayShootSound(); 
                }
            }
        } else {
            game.player2.isAlive = false; 
        }

        game.Draw();  
    }   

    game.Close(); 
    net.Cleanup(); 
    return 0;
}