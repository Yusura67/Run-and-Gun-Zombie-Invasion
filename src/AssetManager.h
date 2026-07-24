// AssetManager.h
#pragma once
#include <vector>
#include "../include/raylib.h"

class AssetManager {
public:
    // โกดังเก็บรูปภาพ (Textures)
    Texture2D bodyTex[7];
    Texture2D playerLegsIdle;
    Texture2D playerLegsWalk[8];
    Texture2D playerLegsJump[3];
    Texture2D bgTex;
    Texture2D ammoBoxTex;
    Texture2D medkitTex;
    Texture2D airplaneTex;
    Texture2D pilotTex[3];
    Texture2D planeFireTex[7];
    Texture2D playerDrinkTex[13];
    Texture2D crateTex[4];
    Texture2D crateExpTex[12];
    
    // Zombie
    Texture2D zombieWalkSheet;
    Texture2D zombieIdleSheet;
    Texture2D zombieAttackSheet;
    Texture2D zombieHurtSheet;
    Texture2D zombieDeathSheet;

    // Final Boss
    Texture2D friendTex;
    Texture2D friendRedEyeTex;

    // Logo game
    Texture2D profileTex;
    Texture2D logoTex;
    Texture2D splashLogoTex;

    Texture2D armPistolTex; // แขนถือปืนพก
    Texture2D armRpgTex;    // แขนแบก RPG
    Texture2D pistolBulletTex[6];
    Texture2D rpgRocketTex;
    Texture2D coinTex;

    // Flying Demon
    Texture2D fdIdleTex;
    Texture2D fdFlyingTex;
    Texture2D fdAttackTex;
    Texture2D fdHurtTex;
    Texture2D fdDeathTex;
    Texture2D fireballTex[5]; // ลูกไฟ

    // Effect
    std::vector<Texture2D> bloodTex[5];
    Texture2D explosionTex;

    // =========================================================
    // โกดังเก็บภาพ Boss End Game
    // =========================================================
    std::vector<Texture2D> bossIdleTex;
    std::vector<Texture2D> bossWalkTex;
    std::vector<Texture2D> bossCleaveTex;
    std::vector<Texture2D> bossTakeHitTex;
    std::vector<Texture2D> bossDeathTex;

    // โกดังเก็บ Sounds
    static const int MAX_SHOOTS = 5;
    Sound shootSounds[MAX_SHOOTS];
    int currentShootIndex;
    
    Sound hitSound;
    Sound deathSound;
    Sound crateBreakSound;
    Sound crateHitSound;
    Sound pickupSound;
    Sound reloadSound;
    Sound playerHurtSound;
    Sound bossWalkSound;
    Sound bossCleaveSound;
    Sound bossScreamSound;
    Sound bossScream2Sound;
    Sound warningSound;
    Sound ironBodySound;
    Sound emptyClickSound;
    Sound rpgShootSound;
    Sound rpgExplosionSound;
    Sound coinPickupSound;
    Sound ammoPickupSound;
    Sound gameOverSound;
    Sound demonFireSound;
    Sound demonScreechSound;
    Sound airplaneArriveSound;
    Sound airplaneDepartSound;
    Sound switchWeaponSound;
    Sound foodPickupSound;
    Sound crateExpSound;

    Music menuMusic;
    Music victoryMusic;
    Music gameplayMusic;
    Music bossMusic;
    Music finalBossMusic;
    Music storyMusic;

    // ฟังก์ชันจัดการโกดัง
    void LoadAll();
    void UnloadAll();
    void PlayShootSound(); // ฟังก์ชันพิเศษช่วยรันคิวเสียงปืน
};