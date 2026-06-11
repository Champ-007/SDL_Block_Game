#pragma once

// So, what does the physics layer need to do?
// 1. manage communication between chunks and player
// 2. manage communication between subsystems and main program.

// Dependencies
// 1. Chunks -> finding collisions
// 2. Player -> sending collisions
// 3. std io -> debugging

#include <iostream>
#include "chunks.hpp"
#include "player.hpp"
#include "collider.cpp"

namespace Physics
{
    void UpdatePlayer(Player& player, ChunkEngine& engine, float dt, const uint8_t* keystates);
}
