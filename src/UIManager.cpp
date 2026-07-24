// UIManager.cpp
#include "UIManager.h"
#include <cmath>

// =================================================================
// หน้าเมนูหลัก (Start Menu)
// =================================================================
void UIManager::DrawMenu() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const char* title = "RUN & GUN: ZOMBIE INVASION";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, sw/2 - titleWidth/2, sh/2 - 130, 60, MAROON); // 👈 ขยับขึ้น

    const char* startText = "Press [ENTER] to Start";
    int startWidth = MeasureText(startText, 30);
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawText(startText, sw/2 - startWidth/2, sh/2 - 40, 30, DARKGRAY); // 👈 ขยับขึ้น
    }

    const char* credits = "Developed by: U-Soup Jamjumrass IT 11";
    int creditsWidth = MeasureText(credits, 20);
    DrawText(credits, sw/2 - creditsWidth/2, sh - 50, 20, BLACK);
    
    // วาดเป้าเล็งเมาส์หน้าเมนู
    Vector2 mousePos = GetMousePosition();

    // วาดปุ่ม CREDITS
    int btnWidth = 200;
    int btnHeight = 50;
    int btnX = sw/2 - btnWidth/2;
    int btnY = sh/2 + 20; // 👈 ขยับขึ้น
    
    Rectangle btnRec = { (float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight };
    
    if (CheckCollisionPointRec(mousePos, btnRec)) {
        DrawRectangleRec(btnRec, GRAY);
        DrawRectangleLinesEx(btnRec, 3, LIME);
    } else {
        DrawRectangleRec(btnRec, DARKGRAY);
    }
    DrawText("CREDITS", btnX + 50, btnY + 15, 20, WHITE);
}

// UIManager.cpp (เวอร์ชันปรับปรุงชุดที่ 2: Upgraded Game Over & Clean Tutorial)
#include "UIManager.h"

void UIManager::DrawHUD(const Player& player) {
    float scale = (float)GetScreenWidth() / 1280.0f;

    float uiX = 20.0f * scale;             
    float uiY = 40.0f * scale;             
    float uiMaxWidth = 200.0f * scale;     
    float uiHeight = 20.0f * scale;        

    // =================================================================
    // 1. ระบบวาดหลอดเลือด (Advanced HP Bar)
    // =================================================================
    float targetHpPercent = (float)player.targetHp / (float)player.maxHp;
    float currentHpPercent = (float)player.displayHp / (float)player.maxHp;
    
    if (currentHpPercent < 0.0f) currentHpPercent = 0.0f; 
    
    Color playerHpColor = LIME; 
    if (currentHpPercent <= 0.5f) playerHpColor = ORANGE; 
    if (currentHpPercent <= 0.2f) playerHpColor = RED;    
    
    // กรอบหลังหลอดเลือด
    DrawRectangle(uiX - (2.0f * scale), uiY - (2.0f * scale), uiMaxWidth + (4.0f * scale), uiHeight + (4.0f * scale), DARKGRAY);
    // พื้นหลังหลอด
    DrawRectangle(uiX, uiY, uiMaxWidth, uiHeight, BLACK); 
    // หลอดสีแดงมืด (แสดงเป้าหมายการฮีลล่วงหน้า)
    DrawRectangle(uiX, uiY, uiMaxWidth * targetHpPercent, uiHeight, MAROON);
    // หลอดสีเขียว (เลือดปัจจุบันที่ค่อยๆ วิ่งตามไป)
    DrawRectangle(uiX, uiY, uiMaxWidth * currentHpPercent, uiHeight, playerHpColor);
    
    int hpFontSize = (int)(16 * scale);
    if (hpFontSize < 10) hpFontSize = 10; 
    DrawText(TextFormat("HP: %d / %d", player.hp, player.maxHp), uiX + (5.0f * scale), uiY + (2.0f * scale), hpFontSize, WHITE);

    // =================================================================
    // 1.1 เอฟเฟกต์เตือนเลือดน้อย (Low HP Vignette) วาดไว้ก่อนไปวาดคะแนน
    // =================================================================
    if (player.isAlive && currentHpPercent <= 0.3f) {
        // สร้างจังหวะชีพจรเต้นด้วยฟังก์ชันคณิตศาสตร์
        float pulse = (sinf(GetTime() * 8.0f) + 1.0f) / 2.0f; 
        
        // วาดแผ่นสีแดงใสๆ คลุมทั้งจอ
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, 0.15f * pulse));
        // วาดกรอบเส้นหนาๆ สีแดงเข้มที่ขอบหน้าจอ
        DrawRectangleLinesEx({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, 20.0f * scale, Fade(RED, 0.4f * pulse));
    }

    // =================================================================
    // 2. ระบบวาดคะแนน (Score Display)
    // =================================================================
    int scoreFontSize = (int)(24 * scale);
    DrawText(TextFormat("SCORE: %d", player.score), uiX + uiMaxWidth + (20.0f * scale), uiY - (2.0f * scale), scoreFontSize, GOLD);

    // =================================================================
    // 3. ระบบวาดจำนวนกระสุนปืน (Ammo Status)
    // =================================================================
    float ammoY = uiY + uiHeight + (15.0f * scale); 
    int wt = player.weaponType; 

    if (player.isReloading) {
        int reloadFontSize = (int)(22 * scale);
        if ((player.reloadTimer / 10) % 2 == 0) {
            DrawText("RELOADING...", uiX, ammoY, reloadFontSize, RED);
        }
    } else {
        int fontSize = (int)(22 * scale); 
        float drawX = uiX; 
        float drawY = ammoY; 
        Color ammoColor = DARKGRAY;
        
        if (player.ammo[wt] == 0 && player.reserveAmmo[wt] == 0) ammoColor = RED;
        else if (player.ammo[wt] <= (wt == 1 ? 0 : 5)) ammoColor = ORANGE; 
        
        if (player.outOfAmmoTimer > 0) { 
            fontSize = (int)(28 * scale); 
            ammoColor = RED; 
            drawX += GetRandomValue(-3, 3) * scale; 
            drawY += GetRandomValue(-3, 3) * scale; 
        }
        
        const char* weaponName = (wt == 1) ? "RPG" : "PISTOL";
        DrawText(TextFormat("%s AMMO: %d / %d", weaponName, player.ammo[wt], player.reserveAmmo[wt]), drawX, drawY, fontSize, ammoColor);
    }

    // =================================================================
    // 4. แถบคู่มือสอนเล่นด้านบนสุด (จะแสดงผลเฉพาะตอนที่ยังปะทะและมีชีวิตอยู่)
    // =================================================================
    if (player.isAlive && !player.isWinner) {
        int tutorialFontSize = (int)(20 * scale);
        DrawText("A/D = Move | Space = Jump | Click = Shoot | R = Reload | O = Pause | F = Fullscreen", 10.0f * scale, 10.0f * scale, tutorialFontSize, DARKGRAY);
    }

    // =================================================================
    // 5. ระบบเป้าเล็งปืน (Crosshair)
    // =================================================================
    Vector2 mousePos = GetMousePosition();
    float chRadius = 8.0f * scale;
    float chLineStart = 6.0f * scale;
    float chLineEnd = 14.0f * scale;
    
    DrawCircleLines(mousePos.x, mousePos.y, chRadius, RED);
    DrawCircle(mousePos.x, mousePos.y, 2.0f * scale, RED);
    DrawLine(mousePos.x - chLineEnd, mousePos.y, mousePos.x - chLineStart, mousePos.y, RED);
    DrawLine(mousePos.x + chLineStart, mousePos.y, mousePos.x + chLineEnd, mousePos.y, RED);
    DrawLine(mousePos.x, mousePos.y - chLineEnd, mousePos.x, mousePos.y - chLineStart, RED);
    DrawLine(mousePos.x, mousePos.y + chLineStart, mousePos.x, mousePos.y + chLineEnd, RED);

    // =================================================================
    // 6. หน้าจอ GAME OVER
    // =================================================================
    if (!player.isAlive) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.75f));
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(MAROON, 0.15f));
        
        // 1. หัวข้อใหญ่ (ขยับขึ้นไปเล็กน้อย)
        int goFontSize = (int)(55 * scale);
        const char* goText = "YOU WERE OVERRUN!";
        int goWidth = MeasureText(goText, goFontSize);
        float titleY = GetScreenHeight() / 2 - (140.0f * scale); 
        
        DrawText(goText, GetScreenWidth() / 2 - goWidth / 2 + (3 * scale), titleY + (3 * scale), goFontSize, BLACK);
        DrawText(goText, GetScreenWidth() / 2 - goWidth / 2, titleY, goFontSize, RED);
        
        // 2. แสดงคะแนนสุดท้าย
        int statFontSize = (int)(26 * scale);
        float statY = GetScreenHeight() / 2 - (50.0f * scale); 
        DrawText(TextFormat("FINAL SCORE: %d", player.score), GetScreenWidth() / 2 - MeasureText(TextFormat("FINAL SCORE: %d", player.score), statFontSize) / 2, statY, statFontSize, WHITE);
        
        // 3. ระดับการเอาชีวิตรอด
        const char* comment = "ZOMBIE FOOD";
        Color commentColor = GRAY;
        if (player.score >= 500) { comment = "LEGENDARY SURVIVOR"; commentColor = GOLD; }
        else if (player.score >= 300) { comment = "HARD TO KILL"; commentColor = ORANGE; }
        else if (player.score >= 150) { comment = "BRAVE SOLDIER"; commentColor = LIME; }
        
        float commentY = GetScreenHeight() / 2 - (10.0f * scale); 
        DrawText(TextFormat("SURVIVAL STATUS: %s", comment), GetScreenWidth() / 2 - MeasureText(TextFormat("SURVIVAL STATUS: %s", comment), statFontSize) / 2, commentY, statFontSize, commentColor);
        
        // 4. สาเหตุการตาย (ขยับมาอยู่ใต้สถานะ และเหนือปุ่ม)
        float causeY = GetScreenHeight() / 2 + (30.0f * scale); 
        const char* causeText = TextFormat("CAUSE OF DEATH: %s", player.deathCause.c_str());
        DrawText(causeText, GetScreenWidth() / 2 - MeasureText(causeText, statFontSize) / 2, causeY, statFontSize, RED);

        // 5. ปุ่มกดลุยใหม่อีกครั้ง (ขยับลงด้านล่างสุดเพื่อหลบตัวอักษร ไม่ให้ทับกันอีกต่อไป)
        int btnWidth = (int)(260 * scale);
        int btnHeight = (int)(55 * scale);
        int btnX = GetScreenWidth() / 2 - btnWidth / 2;
        int btnY = GetScreenHeight() / 2 + (90.0f * scale);
        
        Rectangle btnRec = { (float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight };
        
        bool isHovering = CheckCollisionPointRec(mousePos, btnRec);
        Color btnColor = isHovering ? MAROON : DARKGRAY;
        Color btnOutline = isHovering ? RED : GRAY;
        Color textColor = isHovering ? WHITE : LIGHTGRAY;

        DrawRectangleRec(btnRec, btnColor);
        DrawRectangleLinesEx(btnRec, 2.0f * scale, btnOutline);
        
        int btnTextSize = (int)(20 * scale);
        const char* btnText = "REDEPLOY (Press P)";
        int btnTextWidth = MeasureText(btnText, btnTextSize);
        DrawText(btnText, btnX + btnWidth / 2 - btnTextWidth / 2, btnY + (btnHeight / 2 - btnTextSize / 2), btnTextSize, textColor);
    }
    
    // =================================================================
    // 7. ระบบหน้าจอเมื่อชนะเกม (Victory Window)
    // =================================================================
    if (player.isWinner) {
        int winFontSize = (int)(40 * scale);
        const char* winText = "YOU WIN!";
        int winWidth = MeasureText(winText, winFontSize);
        DrawText(winText, GetScreenWidth() / 2 - winWidth / 2, GetScreenHeight() / 2 - (30.0f * scale), winFontSize, GREEN);
        
        int winAgainFontSize = (int)(20 * scale);
        const char* winAgainText = "Press 'P' to Play Again";
        int winAgainWidth = MeasureText(winAgainText, winAgainFontSize);
        DrawText(winAgainText, GetScreenWidth() / 2 - winAgainWidth / 2, GetScreenHeight() / 2 + (20.0f * scale), winAgainFontSize, DARKGRAY);
    }
}

// หน้าจอ Credit
void UIManager::DrawCredits(Texture2D profileTex, Texture2D logoTex) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("DEVELOPER CREDITS", sw/2 - MeasureText("DEVELOPER CREDITS", 50)/2, 80, 50, MAROON);

    // วาดรูปโปรไฟล์และโลโก้
    DrawTexture(profileTex, sw/2 - profileTex.width - 20, 180, WHITE);
    DrawTexture(logoTex, sw/2 + 20, 180, WHITE);

    // ข้อมูลผม
    DrawText("U-Soup Jamjumrass", sw/2 - MeasureText("U-Soup Jamjumrass", 40)/2, 450, 40, DARKGRAY);
    DrawText("Student ID: 67219010011", sw/2 - MeasureText("Student ID: 67219010011", 20)/2, 500, 20, GRAY);
    DrawText("Information Technology - Game Development", sw/2 - MeasureText("Information Technology - Game Development", 20)/2, 530, 20, GRAY);

    // ปุ่มย้อนกลับ
    int btnWidth = 200;
    int btnHeight = 50;
    int btnX = sw/2 - btnWidth/2;
    int btnY = sh - 100;
    
    Rectangle btnRec = { (float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight };
    Vector2 mousePos = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePos, btnRec)) {
        DrawRectangleRec(btnRec, GRAY);
        DrawRectangleLinesEx(btnRec, 3, RED);
    } else {
        DrawRectangleRec(btnRec, DARKGRAY);
    }
    DrawText("BACK", btnX + 70, btnY + 15, 20, WHITE);
}

void UIManager::DrawVictory(const Player& player) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // 1. พื้นหลัง
    DrawRectangle(0, 0, sw, sh, Fade(GOLD, 0.2f));
    
    DrawText("MISSION ACCOMPLISHED", sw/2 - MeasureText("MISSION ACCOMPLISHED", 60)/2, 100, 60, DARKGREEN);

    // 2. แสดงรายละเอียดคะแนน
    int panelY = 220;
    DrawText(TextFormat("FINAL SCORE: %d", player.score), sw/2 - 150, panelY, 30, BLACK);
    DrawText(TextFormat("HEALTH REMAINING: %d", player.hp), sw/2 - 150, panelY + 50, 30, BLACK);

    // 3. ลอจิกการตัดเกรด (Ranking)
    const char* rank = "C";
    Color rankColor = GRAY;

    if (player.score >= 500 && player.hp >= 8) { rank = "S"; rankColor = GOLD; }
    else if (player.score >= 300) { rank = "A"; rankColor = ORANGE; }
    else if (player.score >= 150) { rank = "B"; rankColor = LIME; }

    DrawText("RANK", sw/2 - 20, panelY + 120, 20, DARKGRAY);
    DrawText(rank, sw/2 - MeasureText(rank, 120)/2, panelY + 150, 120, rankColor);

    // 4. ปุ่มกลับหน้าเมนู (แบบใช้เมาส์คลิก)
    int btnWidth = 250;
    int btnHeight = 60;
    int btnX = sw/2 - btnWidth/2;
    int btnY = sh - 120;
    
    Rectangle btnRec = { (float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight };
    Vector2 mousePos = GetMousePosition();
    
    if (CheckCollisionPointRec(mousePos, btnRec)) {
        DrawRectangleRec(btnRec, DARKGREEN);
        DrawRectangleLinesEx(btnRec, 3, GREEN);
    } else {
        DrawRectangleRec(btnRec, MAROON);
    }
    DrawText("RETURN TO HQ", btnX + 45, btnY + 20, 22, WHITE);
}

void UIManager::DrawSplash(int splashTimer, Texture2D splashLogoTex, Texture2D deptLogoTex) {
    ClearBackground(BLACK);

    if (splashTimer <= 210) {
        float alpha = 0.0f;
        if (splashTimer < 60) alpha = splashTimer / 60.0f;
        else if (splashTimer < 150) alpha = 1.0f;
        else alpha = 1.0f - ((splashTimer - 150) / 60.0f);

        if (splashLogoTex.id != 0) { 
            float drawX = (GetScreenWidth() - splashLogoTex.width) / 2.0f;
            float drawY = (GetScreenHeight() - splashLogoTex.height) / 2.0f;
            DrawTexture(splashLogoTex, drawX, drawY, Fade(WHITE, alpha)); 
        }
    } 
    else {
        int timer2 = splashTimer - 210; 
        float alpha = 0.0f;
        
        if (timer2 < 60) alpha = timer2 / 60.0f;
        else if (timer2 < 150) alpha = 1.0f;
        else alpha = 1.0f - ((timer2 - 150) / 60.0f);

        const char* text1 = "DEVELOPED BY Yusoup Jamjumrass"; 
        const char* text2 = "INFORMATION TECHNOLOGY DEPARTMENT"; 

        int text1Width = MeasureText(text1, 30);
        int text2Width = MeasureText(text2, 30);

        if (deptLogoTex.id != 0) {
            float scale = 1.0f; 
            float logoW = deptLogoTex.width * scale;
            float logoH = deptLogoTex.height * scale;
            
            float logoX = (GetScreenWidth() - logoW) / 2.0f;
            float logoY = (GetScreenHeight() - logoH) / 2.0f;

            DrawText(text1, (GetScreenWidth() - text1Width) / 2, logoY - 50, 30, Fade(LIGHTGRAY, alpha));

            Rectangle srcRec = { 0, 0, (float)deptLogoTex.width, (float)deptLogoTex.height };
            Rectangle destRec = { logoX, logoY, logoW, logoH };
            DrawTexturePro(deptLogoTex, srcRec, destRec, {0,0}, 0.0f, Fade(WHITE, alpha));

            DrawText(text2, (GetScreenWidth() - text2Width) / 2, logoY + logoH + 30, 30, Fade(WHITE, alpha));
        } 
        else {
            DrawText(text1, (GetScreenWidth() - text1Width) / 2, GetScreenHeight() / 2 - 50, 30, Fade(LIGHTGRAY, alpha));
            DrawText(text2, (GetScreenWidth() - text2Width) / 2, GetScreenHeight() / 2 + 10, 30, Fade(WHITE, alpha));
        }
    }
}

void UIManager::DrawStory(Texture2D storyTex, Texture2D zombieIdleSheet) {
    Rectangle src = { 0, 0, (float)storyTex.width, (float)storyTex.height };
    Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(storyTex, src, dest, {0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();
    Rectangle btnVideoTopRight = { (float)GetScreenWidth() - 250.0f, 40.0f, 200.0f, 50.0f };
    
    bool isHoveringVideo = CheckCollisionPointRec(mousePos, btnVideoTopRight);
    Color vBtnColor = isHoveringVideo ? LIGHTGRAY : DARKGRAY;
    Color vTextColor = isHoveringVideo ? BLACK : WHITE;
    Color vOutlineColor = isHoveringVideo ? WHITE : GRAY;

    DrawRectangleRec(btnVideoTopRight, vBtnColor);
    DrawRectangleLinesEx(btnVideoTopRight, 2, vOutlineColor);
    
    int vTextWidth = MeasureText("PLAY VIDEO", 20);
    DrawText("PLAY VIDEO", btnVideoTopRight.x + (btnVideoTopRight.width/2) - (vTextWidth/2), btnVideoTopRight.y + 15, 20, vTextColor);

    int idleFrames = 6; 
    int currentZombieFrame = (int)(GetTime() * 6.0f) % idleFrames; 
    float zFrameWidth = (float)zombieIdleSheet.width / idleFrames;
    float zTotalHeight = (float)zombieIdleSheet.height;

    Rectangle zSrcRec = { currentZombieFrame * zFrameWidth, 0, zFrameWidth, zTotalHeight };
    float zScale = 3.0f; 
    float zDestWidth = zFrameWidth * zScale;
    float zDestHeight = zTotalHeight * zScale;

    Rectangle zDestRec = { 0.1f, (float)GetScreenHeight() - zDestHeight - 1.0f, zDestWidth, zDestHeight };
    DrawTexturePro(zombieIdleSheet, zSrcRec, zDestRec, {0, 0}, 0.0f, WHITE);

    float blinkAlpha = (sinf(GetTime() * 6.0f) + 1.0f) / 2.0f;
    const char* backText = "PRESS [ENTER] TO RETURN TO MENU";
    int backTextWidth = MeasureText(backText, 24);
    
    DrawText(backText, (GetScreenWidth() / 2) - (backTextWidth / 2), GetScreenHeight() - 60, 24, Fade(GRAY, blinkAlpha));
}

void UIManager::DrawLobby(std::string ipInput, bool typingIp) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("MULTIPLAYER LOBBY", sw/2 - MeasureText("MULTIPLAYER LOBBY", 50)/2, sh/2 - 150, 50, MAROON);

    Vector2 mousePos = GetMousePosition();

    // 1. ปุ่ม HOST GAME (ขยายกว้าง 360, ขยับ Y ขึ้น)
    Rectangle hostBtn = { sw/2.0f - 180, sh/2.0f - 70, 360, 50 };
    bool hHover = CheckCollisionPointRec(mousePos, hostBtn);
    DrawRectangleRec(hostBtn, hHover ? MAROON : DARKGRAY);
    DrawRectangleLinesEx(hostBtn, 2, hHover ? RED : GRAY);
    DrawText("HOST GAME (Create Server)", hostBtn.x + 180 - MeasureText("HOST GAME (Create Server)", 20)/2, hostBtn.y + 15, 20, WHITE);

    // 2. ปุ่ม JOIN GAME
    Rectangle joinBtn = { sw/2.0f - 180, sh/2.0f, 360, 50 };
    bool jHover = CheckCollisionPointRec(mousePos, joinBtn);
    DrawRectangleRec(joinBtn, jHover ? DARKGREEN : DARKGRAY);
    DrawRectangleLinesEx(joinBtn, 2, jHover ? LIME : GRAY);
    DrawText("JOIN GAME (Connect to IP)", joinBtn.x + 180 - MeasureText("JOIN GAME (Connect to IP)", 20)/2, joinBtn.y + 15, 20, WHITE);

    // ⌨3. กล่องพิมพ์ IP Address (ขยับลงให้พ้นปุ่ม)
    DrawText("Server IP:", sw/2.0f - 150, sh/2.0f + 85, 20, DARKGRAY);
    Rectangle ipBox = { sw/2.0f - 40, sh/2.0f + 75, 200, 40 };
    DrawRectangleRec(ipBox, typingIp ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx(ipBox, 2, typingIp ? RED : GRAY);
    DrawText(ipInput.c_str(), ipBox.x + 10, ipBox.y + 10, 20, DARKGRAY);

    // 4. ปุ่ม BACK
    Rectangle backBtn = { sw/2.0f - 100, sh/2.0f + 160, 200, 50 };
    bool bHover = CheckCollisionPointRec(mousePos, backBtn);
    DrawRectangleRec(backBtn, bHover ? GRAY : DARKGRAY);
    DrawRectangleLinesEx(backBtn, 2, bHover ? RED : GRAY);
    DrawText("BACK TO MENU", backBtn.x + 100 - MeasureText("BACK TO MENU", 20)/2, backBtn.y + 15, 20, WHITE);
}