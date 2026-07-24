// Boss.cpp
#include "Boss.h"

// กำหนดค่าเริ่มต้นของบอส
Boss::Boss() {
    active = false; 
    width = 150.0f; 
    height = 150.0f;
    speed = 2.2f;   
    velocityY = 0;
    maxHp = 100;    
    hp = maxHp;     
    facingRight = false;
    state = BossState::IDLE;
    
    currentFrame = 0;
    frameCounter = 0;
    attackCooldownTimer = 0;
    stateTimer = 0;
    isEnraged = false;
}

void Boss::Draw(AssetManager& assets, bool isPhase2Cutscene, int cutsceneTimer) {
    Texture2D tex = {0};
    
    // เลือก Texture ตามสถานะของบอส
    if (state == BossState::IDLE && assets.bossIdleTex.size() > 0)          tex = assets.bossIdleTex[currentFrame % assets.bossIdleTex.size()];
    else if (state == BossState::WALK && assets.bossWalkTex.size() > 0)     tex = assets.bossWalkTex[currentFrame % assets.bossWalkTex.size()];
    else if (state == BossState::CLEAVE && assets.bossCleaveTex.size() > 0)   tex = assets.bossCleaveTex[currentFrame % assets.bossCleaveTex.size()];
    else if (state == BossState::TAKE_HIT && assets.bossTakeHitTex.size() > 0) tex = assets.bossTakeHitTex[currentFrame % assets.bossTakeHitTex.size()];
    else if ((state == BossState::DEAD || state == BossState::SUMMON) && assets.bossDeathTex.size() > 0) {
        int drawFrame = currentFrame;
        
        // Ping-Pong Animation สำหรับท่า Summon
        if (state == BossState::SUMMON && currentFrame > 8) {
            drawFrame = 16 - currentFrame;
        }
        
        if (drawFrame < 0) drawFrame = 0;
        if (drawFrame >= assets.bossDeathTex.size()) drawFrame = assets.bossDeathTex.size() - 1;
        
        tex = assets.bossDeathTex[drawFrame];
    }

    // เริ่มขั้นตอนการวาดภาพบอสลงจอ
    if (tex.id != 0) { 
        Rectangle srcRec = { 0, 0, (float)tex.width, (float)tex.height };
        if (facingRight) srcRec.width = -srcRec.width;

        // คำนวณขนาดตัว (Scale) ของบอสตามสถานะและคัตซีน
        float scale = 1.5f;
        if (isEnraged) {
            scale = 4.5f; 
        }
        else if (hp <= maxHp / 2) {
            if (isPhase2Cutscene) {
                float progress = 1.0f - ((float)cutsceneTimer / 180.0f); 
                scale = 1.5f + (1.0f * progress);
            } else {
                scale = 2.5f; 
            }
        }

        float drawWidth = tex.width * scale;
        float drawHeight = tex.height * scale;
        
        Vector2 origin = { drawWidth / 2.0f, drawHeight }; 
        Rectangle destRec = { x + width / 2.0f, y + height, drawWidth, drawHeight };
        
        // การจัดการสี (Color Tint) ของบอส
        Color bossTint = WHITE; 
        if (isEnraged) {
            bossTint = RED; 
        }
        else if (hp > maxHp / 2 && stateTimer >= 180) {
            float intensity = 0.0f; 
            if (stateTimer < 240) intensity = (stateTimer - 180) / 60.0f; 
            else if (stateTimer < 330) intensity = 1.0f - ((stateTimer - 240) / 90.0f); 
            
            unsigned char gb = (unsigned char)(255 - (120 * intensity)); 
            bossTint = { 255, gb, gb, 255 }; 
        }

        DrawTexturePro(tex, srcRec, destRec, origin, 0.0f, bossTint);
    }
}