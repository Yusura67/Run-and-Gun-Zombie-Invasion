// FlyingDemon.h
#pragma once
#include "../include/raylib.h"
#include "AssetManager.h"
#include "Player.h"
#include "Props.h"
#include <vector>

enum class DemonState { IDLE, FLYING, ATTACK, HURT, DEAD };

class FlyingDemon {
public:
    float x, y, width, height;
    float speed;
    int hp, maxHp;
    bool active;
    bool facingRight;

    DemonState state;
    int currentFrame;
    int frameCounter;
    int stateTimer;
    int attackCooldown;

    // Constructor
    FlyingDemon(float startX, float startY);

    void Update(Player& player, std::vector<Bullet>& bullets, AssetManager& assets);
    void Draw(AssetManager& assets);
};