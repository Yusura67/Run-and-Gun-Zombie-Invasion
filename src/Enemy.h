// Enemy.h
#pragma once
#include "../include/raylib.h"
#include "AssetManager.h"

class Player;
class Item;

enum class ZombieState { IDLE, WALK, ATTACK, HURT, DEAD };

class Enemy {
public:
    int id;
    float x, y, width, height;
    bool active;
    float speed;
    bool facingRight;
    float velocityY;
    int currentFrame;   
    int frameCounter; 
    int hp;  
    int maxHp;
    int attackCooldownTimer; 
    ZombieState state;
    int stateTimer;

    // onstructor: ฟังก์ชันที่จะถูกเรียกอัตโนมัติเมื่อเสกซอมบี้
    Enemy(float startX, float startY, float moveSpeed);

    void Draw(AssetManager& assets);
    void Update(Player& player, const std::vector<Rectangle>& platforms, float gravity, std::vector<Item>& items, AssetManager& assets);
};