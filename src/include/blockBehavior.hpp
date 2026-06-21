#pragma once

#include <cstdint>
#include <string>

#include "blockDataItem.hpp"

using BlockID = uint16_t;
using BlockData = uint8_t;

struct Chunk; // Forward declaration

struct BehaviorContext
{
    int x;
    int y;
    BlockID block;
    BlockData* data;
    Chunk* chunk;

    BehaviorContext() = default;
    BehaviorContext(int x, int y, BlockID block, BlockData* data, Chunk* chunk);
};

// The virtual prototype for all other block behaviors
struct BlockBehavior
{
    virtual ~BlockBehavior() = default;
    virtual std::vector<DataItemID> Init() = 0;
    virtual BehaviorContext OnUpdate(BehaviorContext ctx) = 0;
    virtual BehaviorContext OnPlace(BehaviorContext ctx) = 0;
    virtual BehaviorContext OnBreak(BehaviorContext ctx) = 0;
};

// Actual behavior declarations

// Classic sand-like behavior. Falls down or at a diagonal
struct FallBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext OnUpdate(BehaviorContext ctx);
    BehaviorContext OnPlace(BehaviorContext ctx);
    BehaviorContext OnBreak(BehaviorContext ctx);
};

// Grass transforms dirt into more grass
struct SpreadGrassBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext OnUpdate(BehaviorContext ctx);
    BehaviorContext OnPlace(BehaviorContext ctx);
    BehaviorContext OnBreak(BehaviorContext ctx);
};

// Blocks fall like sand, but also move left and right randomly.
struct LiquidFlowBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext OnUpdate(BehaviorContext ctx);
    BehaviorContext OnPlace(BehaviorContext ctx);
    BehaviorContext OnBreak(BehaviorContext ctx);
};

// Liquids look down and at diagonals to combine with same blocks
struct LiquidCombineBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext OnUpdate(BehaviorContext ctx);
    BehaviorContext OnPlace(BehaviorContext ctx);
    BehaviorContext OnBreak(BehaviorContext ctx);
};

// Block checks four adjacent blocks for water, turns to obsidian if true
struct LavaToObsidianBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext OnUpdate(BehaviorContext ctx);
    BehaviorContext OnPlace(BehaviorContext ctx);
    BehaviorContext OnBreak(BehaviorContext ctx);
};
