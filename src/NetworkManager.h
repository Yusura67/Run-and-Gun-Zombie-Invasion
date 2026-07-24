// NetworkManager.h
#pragma once
#include <string>
#include <vector>

// โครงสร้างข้อมูลสำหรับรอเสกไอเทม
struct NetItemSpawn {
    float x, y;
    int type;
    int id;
};

struct NetEnemyData {
    int id;
    float x, y;
    bool facingRight;
    bool active;
    int hp;
    int state;
    int currentFrame;
};

struct GamePacket {
    int packetType; // 0=Player, 1=ItemDel, 2=ItemSpawn, 3=EnemySync

    // ข้อมูลผู้เล่น (packetType == 0)
    float x, y; bool facingRight; float rotation; int weaponType;      
    bool isMoving; bool isGrounded; int currentLegFrame; bool isAlive; bool justShot;

    // ข้อมูลไอเทม (packetType == 1 และ 2)
    int targetId; float itemX, itemY; int itemType;

    // ข้อมูลซอมบี้ (ใช้ตอน packetType == 3)
    int enemyId;
    float enemyX, enemyY;
    bool enemyFacingRight;
    bool enemyActive;
    int enemyHp;
    int enemyState;
    int enemyFrame;
};

class NetworkManager {
private:
    void* host; 
    void* peer; 

public:
    GamePacket otherPlayer;      
    bool hasOtherPlayer = false; 
    std::vector<int> itemsToDestroy; 
    std::vector<NetItemSpawn> itemsToSpawn;
    std::vector<NetEnemyData> enemiesToSync;

    NetworkManager();
    bool Initialize();
    void Cleanup();

    bool CreateServer(int port, int maxClients);
    bool JoinServer(std::string ipAddress, int port);
    void Update();

    void SendPlayerSync(float x, float y, bool facingRight, float rotation, int weaponType, bool isMoving, bool isGrounded, int currentLegFrame, bool isAlive, bool justShot);
    void SendItemDestroyed(int itemId);

    void SendItemSpawn(float x, float y, int type, int id);
    void SendEnemySync(int id, float x, float y, bool facingRight, bool active, int hp, int state, int currentFrame);
};