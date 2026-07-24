// Game.h
#pragma once
#include "../include/raylib.h"
#include <vector>
#include <string>
#include <cstdio>

#include "Player.h"
#include "Enemy.h"
#include "Props.h"
#include "Boss.h"
#include "UIManager.h"
#include "AssetManager.h"
#include "FlyingDemon.h"

class Game {
public:
    Game();
    void Init();
    void Reset();
    void Update();
    void Draw();
    bool ShouldClose() const;
    void Close();

    Player player;
    Player player2;

    std::vector<Bullet> bullets;
    std::vector<Item> items;
    AssetManager assets;
    bool isHost = true;

    std::vector<int> recentlyCollectedItems;

    // =========================================================
    // ระบบเอฟเฟคตัวหนังสือลอย (Floating Text)
    // =========================================================
    struct FloatingText {
        float x, y;
        std::string text;
        Color color;
        int lifeTimer;
        int maxLife;
        float speedY;

        void Update() {
            y -= speedY; // ลอยขึ้นทีละนิด
            lifeTimer--; // ลดเวลาลงเรื่อยๆ
        }

        void Draw() const {
            if (lifeTimer <= 0) return;
            
            // คำนวณความจาง (Alpha) ตัวหนังสือจะค่อยๆ จางลงตอนใกล้หมดเวลา
            float alpha = (float)lifeTimer / (float)maxLife;
            Color drawColor = Fade(color, alpha);
            Color shadowColor = Fade(BLACK, alpha);
            
            // วาดเงาสีดำให้ตัวหนังสือ เพื่อให้อ่านง่ายเวลาฉากหลังสว่าง
            DrawText(text.c_str(), (int)x + 2, (int)y + 2, 24, shadowColor);
            // วาดตัวหนังสือจริง
            DrawText(text.c_str(), (int)x, (int)y, 24, drawColor);
        }
    };
    std::vector<FloatingText> floatingTexts;

    // =========================================================
    // ตัวแปรของเอนจิ้นและวัตถุในเกม (Engine & Entities)
    // =========================================================
    std::vector<Enemy> enemies;
    std::vector<Crate> crates;

    Boss boss;

    std::vector<Rectangle> platforms;
    std::vector<FlyingDemon> flyingDemons;
    Rectangle goal;
    Camera2D camera;
    UIManager uiManager;
    
    // =========================================================
    // ระบบ Lobby ออนไลน์
    // =========================================================
    bool requestHost = false;
    bool requestJoin = false;
    std::string joinIp = "127.0.0.1";
    bool typingIp = false;

private:
    // =========================================================
    // ตัวแปรระบบ
    // =========================================================
    int cameraShakeTimer = 0;
    int screenFlashTimer = 0;
    int supplyDropTimer = 0;
    int phase3Delay = 0;
    int splashTimer = 0;
    bool isPaused = false;
    bool gameOverSoundPlayed = false;

    // ตัวแปรควบคุมคัดซีนเครื่องบิน
    float airplaneX, airplaneY;
    int airplanePhase; // 0 = บินลง, 1 = ปล่อยคน, 2 = ชูนิ้วโป้ง, 3 = บินหนี
    int pilotFrame;
    int pilotTimer;
    int planeFireFrame;
    int planeFireTimer;

    // =========================================================
    // ระบบควบคุมสถานะเกม (Game State)
    // =========================================================
    enum class GameState {
        SPLASH,
        INTRO_VIDEO,
        MENU,
        PLAYING,
        CREDITS,
        VICTORY,
        STORY,
        SURVIVAL,
        LOBBY
    };
    GameState currentState; // เก็บสถานะว่าอยู่หน้าเมนูหรือหน้าเล่นเกม

    // =========================================================
    // ระบบระเบิด (Explosion System)
    // =========================================================
    struct ExplosionEffect {
        float x, y;
        int currentFrame;   // เฟรมปัจจุบัน (0 ถึง 7)
        int frameCounter;   // ตัวนับเวลาเล่นแอนิเมชัน
        bool active;        // สถานะทำงาน

        // ฟังก์ชันอัปเดตแอนิเมชัน
        void Update() {
            if (!active) return;
            
            frameCounter++;
            if (frameCounter >= 3) { 
                frameCounter = 0;
                currentFrame++;
                if (currentFrame >= 8) { 
                    active = false;
                }
            }
        }
        void Draw(AssetManager& assets) const {
            if (!active || currentFrame >= 8) return;

            Texture2D tex = assets.explosionTex; // ดึงรูประเบิดมา

            // คำนวณตัดแบ่ง 8 ช่องแนวนอน
            float frameWidth = (float)tex.width / 8.0f;
            float frameHeight = (float)tex.height;

            // เลื่อนกรอบ srcRec ไปตามเฟรมปัจจุบัน (Y อยู่ที่ 0 เสมอเพราะแถวเดียว)
            Rectangle srcRec = { currentFrame * frameWidth, 0, frameWidth, frameHeight };

            // ตั้งค่าขนาดระเบิดในเกม
            float scale = 3.0f; // ขนาดความใหญ่ของ effect ระเบิด
            Rectangle destRec = { x, y, frameWidth * scale, frameHeight * scale };
            
            // ให้จุดหมุนอยู่ตรงกลางรูป
            Vector2 origin = { (frameWidth * scale) / 2.0f, (frameHeight * scale) / 2.0f };

            // วาดระเบิด
            DrawTexturePro(tex, srcRec, destRec, origin, 0.0f, WHITE);
        }
    };
    std::vector<ExplosionEffect> explosions;

    // =========================================================
    // ระบบเอฟเฟคเลือด
    // =========================================================
    struct BloodEffect {
        float x, y;
        int type;         
        int currentFrame;
        int frameCounter;
        bool facingRight; 

        void Update() {
            frameCounter++;
            if (frameCounter >= 2) { 
                frameCounter = 0;
                currentFrame++;
            }
        }

        void Draw(AssetManager& assets) const {
            int maxFrames = assets.bloodTex[type].size(); // เช็คว่าเลือด type นี้มีกี่รูป
            if (currentFrame >= maxFrames) return; 

            Texture2D tex = assets.bloodTex[type][currentFrame]; 

            Rectangle srcRec = { 0, 0, (float)tex.width, (float)tex.height };
            if (!facingRight) srcRec.width = -srcRec.width; 

            float scale = 1.5f; // ปรับขนาดความอลังการของเลือดได้ตามชอบ
            Rectangle destRec = { x, y, tex.width * scale, tex.height * scale };
            Vector2 origin = { (tex.width * scale) / 2.0f, (tex.height * scale) / 2.0f };

            DrawTexturePro(tex, srcRec, destRec, origin, 0.0f, WHITE);
        }
    };
    std::vector<BloodEffect> bloodEffects;

    // =========================================================
    // ระบบหน้าเนื้อเรื่อง (Story Screen)
    // =========================================================
    Texture2D storyTex;
    Rectangle btnReplayVideo; // พื้นที่ปุ่มกดดูวิดีโอ
    Rectangle btnStartGame;   // พื้นที่ปุ่มเข้าสู่เมนูหลัก

    Rectangle btnGoToStory; // พื้นที่ปุ่มเข้าสู่หน้าเนื้อเรื่อง

    // =========================================================
    // ค่าคงที่ของฟิสิกส์ระบบเกม (Constants)
    // =========================================================
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const float gravity = 0.5f;
    const float jumpForce = -10.0f;

    // =========================================================
    // ระบบควบคุมคัตซีน (Cutscene System)
    // =========================================================
    enum class CutsceneType { NONE, WARNING, INTRO, PHASE2, PHASE3_INTRO, SURVIVAL_INTRO, AIRPLANE_DROP };
    CutsceneType currentCutscene = CutsceneType::NONE;
    int cutsceneTimer = 0;
    bool phase2Triggered = false;

    bool isPhase3Escape = false;
    float friendX, friendY, friendScale;
    bool friendRedEye;

    // =========================================================
    // ตัวแปรระบบเครื่องบินกู้ภัย (Phase 3 Escape)
    // =========================================================
    float escapePlaneX, escapePlaneY;
    float escapePlaneBaseY; 
    int escapePlaneState; // 0 = ว่าง, 1 = บินโฉบไปรอ, 2 = จอดรอที่เส้นชัย, 3 = เห็นผู้เล่นแล้วเล่นอนิเมชัน
    int escapePilotFrame;
    int escapePilotTimer;

    // =========================================================
    // ตัวแปรระบบ Survival Wave
    // =========================================================
    int currentWave = 0;
    int enemiesRemaining = 0;
    int enemiesToSpawn = 0;
    int waveSpawnTimer = 0;
    int waveAnnounceTimer = 0;

    // =========================================================
    // ฟังก์ชันย่อยจัดการ State ต่างๆ (Refactored)
    // =========================================================
    void UpdateAudio(); // ระบบจัดการเสียงเพลง
    
    // โซน Update ลอจิก
    void UpdateSplash();
    void UpdateStory();
    void UpdateMenu();
    void UpdateCredits();
    void UpdatePlaying();
    void UpdateSurvival();
    void UpdateVictory();
    void UpdateLobby();

    // โซน Draw วาดภาพ
    void DrawSplash();
    void DrawStory();
    void DrawMenu();
    void DrawCredits();
    void DrawPlaying();
    void DrawSurvival();
    void DrawVictory();
    void DrawLobby();

    // --- โซน Update สำหรับคำนวณ Logic เกม ---
    void UpdateTimers();           // ลดเวลาคูลดาวน์/อมตะ/ตัวหนังสือสั่น ทั้งหมด
    void UpdateAirplaneIntro();   // โค้ดจัดการคัดซีนเครื่องบิน
    void UpdateBullets();           // จัดการพิกัดกระสุนวิ่งและการลบกระสุนเมื่อวิ่งไกลเกิน
    void UpdateEnemies();           // จัดการ AI ซอมบี้ตามสถานะ (เดิน/กัด/ชะงัก/ตาย) และการชน
    void UpdateItems();             // จัดการลอจิกการเดินชนเก็บของดรอปบนพื้น
    void UpdateCrates();            // จัดการลอจิกถังไม้ เช็คการโดนยิง และสุ่มดรอปไอเทม
    void UpdateCamera();            // จัดการกล้องแบบขอบสนาม (Camera Dead Zone) เลื่อนเมื่อชิดขอบจอ
    void UpdateBoss();            // จัดการลอจิกบอส (เดิน/โจมตี/ชะงัก/ตาย) และการชน
    void TriggerExplosion(float x, float y);       

    // --- โซน Draw สำหรับการวาดภาพแสดงผล ---
    void DrawWorld();           // วาดพื้นหลัง, แพลตฟอร์ม, และเป้าหมาย
};