// Props.h
#pragma once
#include "../include/raylib.h"
#include "AssetManager.h"

class AssetManager;

// คลาสกระสุนปืน
class Bullet {
public:
    float x, y, width, height;
    float speedX, speedY;
    bool active;
    int type;

    float angle;      // องศาการหันหน้าของหัวกระสุน
    int currentFrame; // เฟรมปัจจุบัน
    int frameCounter; // ตัวนับเวลา
    bool isHit;

    bool isEnemyBullet;
    
    Bullet(float startX, float startY, float dx, float dy, int bulletType, bool enemyBullet = false);

    void Update(); 
    void Hit(); 
    void Draw(AssetManager& assets);
};

// คลาสถังไม้
class Crate {
public:
    float x, y, width, height;
    int hp;
    bool active;
    
    // ตัวแปรระบบแอนิเมชันที่เพิ่มเข้ามาใหม่
    bool isBroken;       // สถานะว่ากล่องแตกแล้วหรือยัง
    int hitFlashTimer;   // เวลาในการกระพริบสีแดงตอนโดนยิง
    int brokenTimer;     // เวลาในการให้เศษกล่องค่อยๆ จางหายไป
    int expFrame;        // เฟรมแอนิเมชันระเบิด (0-4)
    int expTimer;        // ตัวหน่วงเวลาให้ระเบิดเล่นช้าลง
    float offsetTopY;    // ให้เศษชิ้นบนลอยขึ้น
    float offsetBottomY; // ให้เศษชิ้นล่างร่วงลง

    Crate(float startX, float startY);
    void Update(); // เพิ่มฟังก์ชันอัปเดตแอนิเมชัน
    void Draw(AssetManager& assets);

    Rectangle GetHitbox() const;
};

// คลาสไอเทมดรอป
class Item {
public:
    int id;
    float x, y, width, height;
    int type; // 0 = สมบัติ, 1 = กระสุน, 2 = ยา
    bool active;
    int lifeTimer;

    int currentFrame;
    int frameCounter;
    
    Item(float startX, float startY, int itemType);
    void Update();
    void Draw(AssetManager& assets);
};