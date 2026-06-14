#pragma once

// So, what does the player code need to do?
// 1. manage inventory
// 2. manage stats like health, stamina, etc.
// 3. manage input & player controller
// 4. player physics, collisions

// What dependencies do we need for the player?
// 1. glm -> vec2
// 2. vector -> inventory, collision point clouds
// 3. std io -> debugging
// 4. SDL_SCAN codes -> input

#include <iostream>
#include <list>
#include <stdint.h>
#include <SDL.h>
#include <cstdint>
#include <cmath>

#include "figures.hpp"
#include "vector2.hpp"
#include "collider.cpp"

using BlockID = uint16_t;

// Player structure
struct Player
{
    private:
    vector2 position;
    float width;
    float height;
    vector2 velocity;
    Collider playerCollider;

    double gravity = 0.001f;
    double jumpForce = 0.38f;
    double moveSpeed = 0.05f;
    double moveFriction = 0.15f;

    unsigned int inventorySlotsMax = 40;
    unsigned int inventoryStackMax = 20;
    std::list<std::pair<BlockID, unsigned int>> inventory;
    unsigned int select = 0;
    bool selectChangeTrig = false;
    vector2 cursor_position;

    int tool = 1;

    float heath = 10;
    int falling = 0;
    int crouch = 0;
    bool curTrig_d = false, curTrig_a = false, curTrig_s = false, curTrig_w = false;
    bool buildTrig = false;
    float mining = 0.0f;
    float mineSpeed = 0.02f;

    public:
    Player(vector2 _pos, float _w, float _h);

    vector2 GetPosition();
    void    SetPosition(vector2 p);

    vector2 GetSize();
    void    SetSize(vector2 s);

    float GetWidth();
    void  SetWidth(float w);

    float GetHeight();
    void  SetHeight(float h);

    Collider& GetCollider();
    void UpdateCollisionX(float depth);
    void UpdateCollisionY(float depth);
    void UpdateCollision(vector2 collision);

    bool GetCursor();
    vector2 GetCursorPosition();

    bool IsMining();
    bool IsBuilding();
    float GetMining();
    void ResetMining(bool t);
    void PickUpItem(BlockID id, int count);

    void UpdateInputY(float dt, const uint8_t* keystates);
    void UpdateInputX(float dt, const uint8_t* keystates);
    void UpdateInput(float dt, const uint8_t* keystates);

    std::list<std::pair<BlockID, unsigned int>>& GetInventory();
    int GetInventorySelect();
    void ResetInventorySelect();

    bool GetVelDir();

};
