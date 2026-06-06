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
#include <vector>
#include <stdint.h>
#include <SDL.h>
#include "figures.hpp"
#include "vector2.hpp"
#include "collider.cpp"

// Player structure
struct Player
{
    private:
    vector2 position;
    float width;
    float height;
    vector2 velocity;
    // std::vector<vector2> collider;
    Collider playerCollider;
    // float dt;

    float gravity = 2.0f;
    float jumpForce = 15.8f;
    float moveSpeed = 2.0f;

    vector2 cursor_position;

    // long double depth_col2;
    // float nums[100];
    // vector2 my_depth;

    // const uint8_t* keystates;
    float heath = 10;
    int falling = 0;
    int crouch = 0;
    bool curTrig_d = false, curTrig_a = false, curTrig_s = false, curTrig_w = false;
    float mining = 0.0f;
    float mineSpeed = 0.5f;

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
    float GetMining();
    void ResetMining(bool t);

    void UpdateInputY(float dt, const uint8_t* keystates);
    void UpdateInputX(float dt, const uint8_t* keystates);
    void UpdateInput(float dt, const uint8_t* keystates);

    bool GetVelDir();

    // void Update(float dt);

    // vector2 GetDepth();
};
