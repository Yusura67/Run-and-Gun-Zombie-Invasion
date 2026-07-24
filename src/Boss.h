// Boss.h
#pragma once
#include "../include/raylib.h"
#include "AssetManager.h"

// สถานะต่างๆ ของบอส
enum class BossState {
    IDLE,
    WALK,
    CLEAVE,
    TAKE_HIT,
    DEAD,
    SUMMON
};

class Boss {
public:
    float x, y;
    float width, height;
    float speed;
    float velocityY;
    
    int hp, maxHp;
    bool active;
    bool facingRight;
    BossState state;

    // ระบบแอนิเมชัน
    int currentFrame;
    int frameCounter;

    // ระบบต่อสู้
    int attackCooldownTimer;
    int stateTimer;

    bool isEnraged;

    // ประกาศ Constructor
    Boss(); 
    
    // ฟังก์ชันวาดบอส
    void Draw(AssetManager& assets, bool isPhase2Cutscene, int cutsceneTimer);
};