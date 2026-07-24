// Player.h
#pragma once
#include "../include/raylib.h"
#include "AssetManager.h"
#include "Props.h"
#include <string>

class Player {
public:
    std::string deathCause;
    
    // ตัวแปรสถานะทั้งหมดของผู้เล่น
    float x, y, width, height;
    int hp, maxHp;
    int targetHp;
    float displayHp;
    int iframeTimer;
    int shootCooldownTimer;
    float speed;
    float velocityY;
    bool facingRight;
    bool isReloading;
    int reloadTimer;
    int outOfAmmoTimer;
    bool isDrinking = false;
    int currentDrinkFrame = 0;
    int drinkFrameCounter = 0;

    // ตัวแปรควบคุมอนิเมชั่นการเดิน
    bool isMoving;
    bool isLegFacingRight;
    bool isGrounded;
    int currentLegFrame;
    int legFrameCounter;


    // =========================================================
    // อาวุธและกระสุน
    // ========================================================
    int ammo[2];
    int maxAmmo[2];
    int reserveAmmo[2];
    // =========================================================

    bool isAlive, isWinner;
    int score;
    float rotation;
    float recoilAngle;
    float knockbackX = 0.0f;
    int weaponType;
    bool justShot = false;

    // ฟังก์ชันจัดการตัวเอง
    void Reset();
    void Draw(AssetManager& assets);

    void UpdateMovement(const std::vector<Rectangle>& platforms, float gravity, float jumpForce, const std::vector<Crate>& crates);
    void UpdateCombat(Camera2D camera, std::vector<Bullet>& bullets, AssetManager& assets);
};