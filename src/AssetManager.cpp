// AssetManager.cpp
#include "AssetManager.h"

void AssetManager::LoadAll() {
    // =========================================================
    // โหลดรูปภาพ
    // =========================================================
    for (int i = 0; i < 7; i++) {
        // [0] มองบนสุด -> [6] มองล่างสุด
        bodyTex[i] = LoadTexture(TextFormat("assets/player_body/body%d.png", i + 1));
    }

    
    for (int i = 0; i < 8; i++) {
        playerLegsWalk[i] = LoadTexture(TextFormat("assets/player_legs/walk%d.png", i + 1));
    }

    for (int i = 0; i < 3; i++) {
        playerLegsJump[i] = LoadTexture(TextFormat("assets/player_legs/jump%d.png", i + 1));
    }

    
    for (int i = 0; i < 3; i++) {
        pilotTex[i] = LoadTexture(TextFormat("assets/pilot/pilot%d.png", i + 1));
    }

    for (int i = 0; i < 7; i++) {
        planeFireTex[i] = LoadTexture(TextFormat("assets/pilot/plane_fire%d.png", i + 1));
    }

    for (int i = 0; i < 13; i++) {
        playerDrinkTex[i] = LoadTexture(TextFormat("assets/player_drinking/drink%d.png", i + 1));
    }

    playerLegsIdle = LoadTexture("assets/player_legs/idle.png");
    bgTex = LoadTexture("assets/bg.png");
    ammoBoxTex = LoadTexture("assets/ammo_box.png");
    medkitTex = LoadTexture("assets/medkit.png");
    airplaneTex = LoadTexture("assets/pilot/airplane.png");
    
    // Zombie
    zombieWalkSheet = LoadTexture("assets/zombie_walk_1.png");
    zombieIdleSheet = LoadTexture("assets/zombie_idle.png");
    zombieAttackSheet = LoadTexture("assets/zombie_attack.png");
    zombieHurtSheet = LoadTexture("assets/zombie_hurt.png");
    zombieDeathSheet = LoadTexture("assets/zombie_death.png");

    // Final Boss
    friendTex = LoadTexture("assets/final_boss/friend.png");
    friendRedEyeTex = LoadTexture("assets/final_boss/friend_redeye.png");

    // Logo
    profileTex = LoadTexture("assets/profile.png");
    logoTex = LoadTexture("assets/logo.png");
    splashLogoTex = LoadTexture("assets/splash_logo.png");

    // RPG
    armPistolTex = LoadTexture("assets/player_body/arm_pistol.png");
    armRpgTex = LoadTexture("assets/player_body/arm_rpg.png");
    rpgRocketTex = LoadTexture("assets/bullets/rpg/rocket.png");

    // Flying Demon
    fdIdleTex = LoadTexture("assets/flying_demon/idle.png");
    fdFlyingTex = LoadTexture("assets/flying_demon/flying.png");
    fdAttackTex = LoadTexture("assets/flying_demon/attack.png");
    fdHurtTex = LoadTexture("assets/flying_demon/hurt.png");
    fdDeathTex = LoadTexture("assets/flying_demon/death.png");

    for (int i = 0; i < 5; i++) {
        fireballTex[i] = LoadTexture(TextFormat("assets/vfx/fireball/fire_ball%d.png", i + 1));
    }

    // items
    coinTex = LoadTexture("assets/items/coin_sheet.png");

    for (int i = 0; i < 6; i++) {
        pistolBulletTex[i] = LoadTexture(TextFormat("assets/bullets/pistol/%d.png", i + 1));
    }

    for (int i = 0; i < 4; i++) {
        crateTex[i] = LoadTexture(TextFormat("assets/crate/crate%d.png", i + 1));
    }

    for (int i = 0; i < 12; i++) {
        crateExpTex[i] = LoadTexture(TextFormat("assets/crate/crate_exp%d.png", i + 1));
    }

    // โหลดภาพ Boss King Demon แบบแยกชิ้นเป็นเฟรมๆ
    for (int i = 1; i <= 6; i++)  bossIdleTex.push_back(LoadTexture(TextFormat("assets/boss/demon_idle_%d.png", i)));
    for (int i = 1; i <= 12; i++) bossWalkTex.push_back(LoadTexture(TextFormat("assets/boss/demon_walk_%d.png", i)));
    for (int i = 1; i <= 15; i++) bossCleaveTex.push_back(LoadTexture(TextFormat("assets/boss/demon_cleave_%d.png", i)));
    for (int i = 1; i <= 5; i++)  bossTakeHitTex.push_back(LoadTexture(TextFormat("assets/boss/demon_take_hit_%d.png", i)));
    for (int i = 1; i <= 22; i++) bossDeathTex.push_back(LoadTexture(TextFormat("assets/boss/demon_death_%d.png", i)));

    // Effect
    explosionTex = LoadTexture("assets/vfx/explosions/explosion_sheet.png");
    int frameCounts[5] = { 22, 21, 20, 14, 29 };

    for (int t = 0; t < 5; t++) {
        for (int f = 0; f < frameCounts[t]; f++) {
            bloodTex[t].push_back(LoadTexture(TextFormat("assets/vfx/blood/blood%d/%d.png", t + 1, f + 1)));
        }
    }
    

    // =========================================================
    // โหลดเสียง
    // =========================================================
    for (int i = 0; i < MAX_SHOOTS; i++) {
        shootSounds[i] = LoadSound("assets/sounds/shoot.mp3");
        SetSoundVolume(shootSounds[i], 0.3f);
    }
    currentShootIndex = 0;

    hitSound = LoadSound("assets/sounds/shoot_hit.mp3");
    SetSoundVolume(hitSound, 1.0f);

    deathSound = LoadSound("assets/sounds/zombie_death.mp3");
    SetSoundVolume(deathSound, 1.0f);

    crateBreakSound = LoadSound("assets/sounds/crate_break.mp3");
    SetSoundVolume(crateBreakSound, 1.0f);

    crateHitSound = LoadSound("assets/sounds/crate_hit.mp3");
    SetSoundVolume(crateHitSound, 2.0f);

    pickupSound = LoadSound("assets/sounds/pickup.mp3");
    SetSoundVolume(pickupSound, 5.0f);

    reloadSound = LoadSound("assets/sounds/reload.mp3");
    SetSoundVolume(reloadSound, 0.5f);

    playerHurtSound = LoadSound("assets/sounds/hurt.mp3");
    SetSoundVolume(playerHurtSound, 1.0f);

    bossWalkSound = LoadSound("assets/sounds/boss_walk.mp3");
    SetSoundVolume(bossWalkSound, 4.0f);

    bossCleaveSound = LoadSound("assets/sounds/boss_cleave.mp3");
    SetSoundVolume(bossCleaveSound, 2.5f);

    bossScreamSound = LoadSound("assets/sounds/boss_scream.mp3");
    SetSoundVolume(bossScreamSound, 1.0f);

    bossScream2Sound = LoadSound("assets/sounds/boss_scream2.mp3");
    SetSoundVolume(bossScream2Sound, 2.0f);

    warningSound = LoadSound("assets/sounds/warning.mp3");
    SetSoundVolume(warningSound, 1.0f);

    ironBodySound = LoadSound("assets/sounds/iron_body.mp3");
    SetSoundVolume(ironBodySound, 2.0f);

    emptyClickSound = LoadSound("assets/sounds/empty_click.mp3"); 
    SetSoundVolume(emptyClickSound, 3.0f);

    rpgShootSound = LoadSound("assets/sounds/rpg_shoot.mp3");
    SetSoundVolume(rpgShootSound, 0.8f);

    rpgExplosionSound = LoadSound("assets/sounds/rpg_explosion.mp3");
    SetSoundVolume(rpgExplosionSound, 1.0f);

    coinPickupSound = LoadSound("assets/sounds/coin_pickup.mp3");
    SetSoundVolume(coinPickupSound, 1.0f);

    ammoPickupSound = LoadSound("assets/sounds/ammo_pickup.mp3");
    SetSoundVolume(ammoPickupSound, 1.0f);

    gameOverSound = LoadSound("assets/sounds/gameover.mp3"); 
    SetSoundVolume(gameOverSound, 3.0f);

    demonFireSound = LoadSound("assets/sounds/demon_fire.mp3");
    SetSoundVolume(demonFireSound, 1.5f);

    demonScreechSound = LoadSound("assets/sounds/demon_screech.mp3");
    SetSoundVolume(demonScreechSound, 1.5f);

    airplaneArriveSound = LoadSound("assets/sounds/airplane_arrive.mp3"); 
    SetSoundVolume(airplaneArriveSound, 1.0f);

    airplaneDepartSound = LoadSound("assets/sounds/airplane_depart.mp3");
    SetSoundVolume(airplaneDepartSound, 1.0f);

    switchWeaponSound = LoadSound("assets/sounds/switch_weapon.mp3");
    SetSoundVolume(switchWeaponSound, 2.0f);

    foodPickupSound = LoadSound("assets/sounds/food_pickup.mp3");
    SetSoundVolume(foodPickupSound, 1.0f);


    // โหลดเพลง
    menuMusic = LoadMusicStream("assets/sounds/menu.mp3");
    SetMusicVolume(menuMusic, 1.5f);

    victoryMusic = LoadMusicStream("assets/sounds/victory.mp3");
    SetMusicVolume(victoryMusic, 1.5f);

    gameplayMusic = LoadMusicStream("assets/sounds/gameplay.mp3");
    SetMusicVolume(gameplayMusic, 0.4f);

    bossMusic = LoadMusicStream("assets/sounds/boss.mp3");
    SetMusicVolume(bossMusic, 1.0f);

    finalBossMusic = LoadMusicStream("assets/sounds/final_boss.mp3");
    SetMusicVolume(finalBossMusic, 1.5f);

    storyMusic = LoadMusicStream("assets/sounds/story_bgm.mp3");
    SetMusicVolume(storyMusic, 1.0f);

    crateExpSound = LoadSound("assets/sounds/crate_explosion.mp3");
    SetSoundVolume(crateExpSound, 1.5f);
}

void AssetManager::UnloadAll() {
    // ลบรูปภาพออกจาก RAM
    UnloadTexture(playerLegsIdle);
    UnloadTexture(bgTex);
    UnloadTexture(ammoBoxTex);
    UnloadTexture(medkitTex);
    UnloadTexture(zombieWalkSheet);
    UnloadTexture(zombieIdleSheet);
    UnloadTexture(zombieAttackSheet);
    UnloadTexture(zombieHurtSheet);
    UnloadTexture(zombieDeathSheet);
    UnloadTexture(profileTex);
    UnloadTexture(logoTex);
    UnloadTexture(friendTex);
    UnloadTexture(friendRedEyeTex);
    UnloadTexture(rpgRocketTex);
    UnloadTexture(coinTex);
    UnloadTexture(splashLogoTex);
    UnloadTexture(fdIdleTex);
    UnloadTexture(fdFlyingTex);
    UnloadTexture(fdAttackTex);
    UnloadTexture(fdHurtTex);
    UnloadTexture(fdDeathTex);
    UnloadTexture(armPistolTex);
    UnloadTexture(armRpgTex);
    UnloadTexture(airplaneTex);

    for (int i = 0; i < 3; i++) UnloadTexture(pilotTex[i]);
    for (int i = 0; i < 7; i++) UnloadTexture(planeFireTex[i]);
    for (int i = 0; i < 4; i++) UnloadTexture(crateTex[i]);
    for (int i = 0; i < 12; i++) UnloadTexture(crateExpTex[i]);
    
    
    for (int i = 0; i < 8; i++) {
        UnloadTexture(playerLegsWalk[i]);
    }

    for (int i = 0; i < 5; i++) {
        UnloadTexture(fireballTex[i]);
    }

    for (int i = 0; i < 6; i++) {
        UnloadTexture(pistolBulletTex[i]);
    }

    for (int i = 0; i < 7; i++) {
        UnloadTexture(bodyTex[i]);
    }

    for (int i = 0; i < 3; i++) {
        UnloadTexture(playerLegsJump[i]);
    }

    for (int i = 0; i < 13; i++) {
        UnloadTexture(playerDrinkTex[i]);
}
    
    // ลบรูปภาพ Boss End Game ออกจาก RAM 
    for (auto& tex : bossIdleTex) UnloadTexture(tex); 
    bossIdleTex.clear();
    
    for (auto& tex : bossWalkTex) UnloadTexture(tex); 
    bossWalkTex.clear();
    
    for (auto& tex : bossCleaveTex) UnloadTexture(tex); 
    bossCleaveTex.clear();
    
    for (auto& tex : bossTakeHitTex) UnloadTexture(tex); 
    bossTakeHitTex.clear();
    
    for (auto& tex : bossDeathTex) UnloadTexture(tex); 
    bossDeathTex.clear();

    //ลบ Effect ออกจาก RAM
    for (int t = 0; t < 5; t++) {
        for (auto& tex : bloodTex[t]) {
            UnloadTexture(tex);
        }
        bloodTex[t].clear();
    }

    UnloadTexture(explosionTex);
    
    // ลบเสียงออกจาก RAM
    for (int i = 0; i < MAX_SHOOTS; i++) UnloadSound(shootSounds[i]);
    UnloadSound(hitSound);
    UnloadSound(deathSound);
    UnloadSound(crateBreakSound);
    UnloadSound(crateHitSound);
    UnloadSound(pickupSound);
    UnloadSound(reloadSound);
    UnloadSound(playerHurtSound);
    UnloadSound(bossWalkSound);
    UnloadSound(bossCleaveSound);
    UnloadSound(bossScreamSound);
    UnloadSound(bossScream2Sound);
    UnloadSound(ironBodySound); 
    UnloadSound(emptyClickSound);
    UnloadSound(rpgShootSound);
    UnloadSound(rpgExplosionSound);
    UnloadSound(coinPickupSound);
    UnloadSound(ammoPickupSound);
    UnloadSound(gameOverSound);
    UnloadSound(demonFireSound);
    UnloadSound(demonScreechSound);
    UnloadSound(airplaneArriveSound);
    UnloadSound(airplaneDepartSound);
    UnloadSound(switchWeaponSound);
    UnloadSound(foodPickupSound);
    UnloadSound(crateExpSound);

    // ลบเสียงเพลงออกจาก RAM
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(victoryMusic);
    UnloadMusicStream(gameplayMusic);
    UnloadMusicStream(bossMusic); 
    UnloadMusicStream(finalBossMusic);
    UnloadMusicStream(storyMusic);
}

void AssetManager::PlayShootSound() {
    PlaySound(shootSounds[currentShootIndex]);
    currentShootIndex++;
    if (currentShootIndex >= MAX_SHOOTS) currentShootIndex = 0;
}