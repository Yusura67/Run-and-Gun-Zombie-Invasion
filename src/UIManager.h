// UIManager.h
#pragma once
#include "../include/raylib.h"
#include "Player.h"

class UIManager {
public:
    // ฟังก์ชันสำหรับวาดหน้าเมนูตอนเริ่มเกม
    void DrawMenu();
    void DrawHUD(const Player& player); 

    // ฟังก์ชั่นสำหรับวาดหน้า Credit
    void DrawCredits(Texture2D profileTex, Texture2D logoTex);
    
    // ฟังก์ชันวาดหน้าสรุปคะแนน
    void DrawVictory(const Player& player);
    void DrawLobby(std::string ipInput, bool typingIp);

    // ฟังก์ชันสำหรับวาดหน้าจอพิเศษต่างๆ
    void DrawSplash(int splashTimer, Texture2D splashLogoTex, Texture2D deptLogoTex);
    void DrawStory(Texture2D storyTex, Texture2D zombieIdleSheet);
};