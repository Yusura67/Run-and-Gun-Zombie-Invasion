// NetworkManager.cpp
#include <enet/enet.h>
#include "NetworkManager.h"
#include <iostream>

NetworkManager::NetworkManager() {
    host = nullptr;
    peer = nullptr;
    hasOtherPlayer = false;

    otherPlayer.x = -1000.0f;
    otherPlayer.y = -1000.0f;
    otherPlayer.facingRight = true;
}

bool NetworkManager::Initialize() {
    if (enet_initialize() != 0) {
        std::cout << "[NETWORK] Error: Failed to initialize ENet!" << std::endl;
        return false;
    }
    return true;
}

void NetworkManager::Cleanup() {
    if (host != nullptr) {
        enet_host_destroy((ENetHost*)host);
    }
    enet_deinitialize();
    std::cout << "[NETWORK] ENet shutdown gracefully." << std::endl;
}

bool NetworkManager::CreateServer(int port, int maxClients) {
    ENetAddress address;
    address.host = ENET_HOST_ANY; // เปิดรับการเชื่อมต่อจากทุก IP
    address.port = port;

    // สร้าง Server
    host = enet_host_create(&address, maxClients, 2, 0, 0);
    if (host == nullptr) {
        std::cout << "[SERVER] Error: Failed to create host!" << std::endl;
        return false;
    }
    std::cout << "[SERVER] Started successfully on port " << port << std::endl;
    return true;
}

bool NetworkManager::JoinServer(std::string ipAddress, int port) {
    // สร้าง Client
    host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (host == nullptr) {
        std::cout << "[CLIENT] Error: Failed to create client host!" << std::endl;
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, ipAddress.c_str());
    address.port = port;

    // พยายามเชื่อมต่อ (ยื่นมือไปจับ)
    peer = enet_host_connect((ENetHost*)host, &address, 2, 0);
    if (peer == nullptr) {
        std::cout << "[CLIENT] Error: Cannot initiate connection." << std::endl;
        return false;
    }

    // รอผลลัพธ์การจับมือ 5 วินาที
    ENetEvent event;
    if (enet_host_service((ENetHost*)host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "[CLIENT] Success: Connected to server " << ipAddress << std::endl;

        hasOtherPlayer = true;

        return true;
    } else {
        enet_peer_reset((ENetPeer*)peer);
        std::cout << "[CLIENT] Error: Connection to " << ipAddress << " failed. Timeout!" << std::endl;
        return false;
    }
}

void NetworkManager::Update() {
    if (!host) return;

    ENetEvent event;
    // เช็คกล่องจดหมายขาเข้าตลอดเวลาว่ามีใครส่งอะไรมาไหม
    while (enet_host_service((ENetHost*)host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                hasOtherPlayer = true; // มีคนเข้ามาแล้ว
                std::cout << "[NETWORK] Someone joined the game!" << std::endl;
                break;
                
            case ENET_EVENT_TYPE_RECEIVE:
                if (event.packet->dataLength == sizeof(GamePacket)) {
                    GamePacket* data = (GamePacket*)event.packet->data;
                    
                    if (data->packetType == 0) {
                        otherPlayer = *data;
                    } 
                    else if (data->packetType == 1) {
                        itemsToDestroy.push_back(data->targetId);
                    }
                    // ถ้า Host ส่งคำสั่งเสกไอเทมมา ให้จดข้อมูลลงสมุดเตรียมเสก
                    else if (data->packetType == 2) {
                        NetItemSpawn newItem;
                        newItem.x = data->itemX;
                        newItem.y = data->itemY;
                        newItem.type = data->itemType;
                        newItem.id = data->targetId;
                        itemsToSpawn.push_back(newItem);
                    }
                    else if (data->packetType == 3) {
                        NetEnemyData e;
                        e.id = data->enemyId;
                        e.x = data->enemyX;
                        e.y = data->enemyY;
                        e.facingRight = data->enemyFacingRight;
                        e.active = data->enemyActive;
                        e.hp = data->enemyHp;
                        e.state = data->enemyState;
                        e.currentFrame = data->enemyFrame;
                        enemiesToSync.push_back(e);
                    }
                }
                enet_packet_destroy(event.packet);
                break;
                
            case ENET_EVENT_TYPE_DISCONNECT:
                hasOtherPlayer = false;
                std::cout << "[NETWORK] Someone left the game." << std::endl;
                event.peer->data = NULL;
                break;
                
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void NetworkManager::SendPlayerSync(float x, float y, bool facingRight, float rotation, int weaponType, bool isMoving, bool isGrounded, int currentLegFrame, bool isAlive, bool justShot) {
    if (!host) return;

    GamePacket data;
    data.packetType = 0; // ระบุว่าเป็นพัสดุส่งข้อมูลตัวละคร
    data.x = x;
    data.y = y;
    data.facingRight = facingRight;
    data.rotation = rotation;
    data.weaponType = weaponType;
    data.isMoving = isMoving;
    data.isGrounded = isGrounded;
    data.currentLegFrame = currentLegFrame;
    data.isAlive = isAlive;
    data.justShot = justShot;

    ENetPacket* packet = enet_packet_create(&data, sizeof(GamePacket), ENET_PACKET_FLAG_UNSEQUENCED);
    if (peer != nullptr) enet_peer_send((ENetPeer*)peer, 0, packet); 
    else enet_host_broadcast((ENetHost*)host, 0, packet); 
}

void NetworkManager::SendItemDestroyed(int itemId) {
    if (!host) return;

    GamePacket data;
    data.packetType = 1; // ระบุว่าเป็นจดหมายแจ้งลบไอเทม
    data.targetId = itemId;

    // ไอเทมหายต้องชัวร์ 100%
    ENetPacket* packet = enet_packet_create(&data, sizeof(GamePacket), ENET_PACKET_FLAG_RELIABLE);
    
    if (peer != nullptr) enet_peer_send((ENetPeer*)peer, 0, packet); 
    else enet_host_broadcast((ENetHost*)host, 0, packet); 
}

void NetworkManager::SendItemSpawn(float x, float y, int type, int id) {
    if (!host) return;

    GamePacket data;
    data.packetType = 2; // รหัส 2 คือสั่งเสกไอเทม
    data.itemX = x;
    data.itemY = y;
    data.itemType = type;
    data.targetId = id;

    // การสร้างไอเทมต้องชัวร์ 100%
    ENetPacket* packet = enet_packet_create(&data, sizeof(GamePacket), ENET_PACKET_FLAG_RELIABLE);
    
    if (peer != nullptr) enet_peer_send((ENetPeer*)peer, 0, packet); 
    else enet_host_broadcast((ENetHost*)host, 0, packet); 
}

void NetworkManager::SendEnemySync(int id, float x, float y, bool facingRight, bool active, int hp, int state, int currentFrame) {
    if (!host) return;

    GamePacket data;
    data.packetType = 3;
    data.enemyId = id;
    data.enemyX = x;
    data.enemyY = y;
    data.enemyFacingRight = facingRight;
    data.enemyActive = active;
    data.enemyHp = hp;
    data.enemyState = state;
    data.enemyFrame = currentFrame;

    // ซอมบี้ขยับทุกเฟรม ส่งข้อมูลรัวๆ
    ENetPacket* packet = enet_packet_create(&data, sizeof(GamePacket), ENET_PACKET_FLAG_UNSEQUENCED);
    if (peer != nullptr) enet_peer_send((ENetPeer*)peer, 0, packet); 
    else enet_host_broadcast((ENetHost*)host, 0, packet); 
}