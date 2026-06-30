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
    virtual BehaviorContext UpdateTick(BehaviorContext ctx) = 0;
    virtual BehaviorContext RandomTick(BehaviorContext ctx) = 0;
    virtual BehaviorContext SmartTick(BehaviorContext ctx) = 0;
};

// Actual behavior declarations

// Classic sand-like behavior. Falls down or at a diagonal
struct FallBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Dirt becomes soil when near water
struct DirtSoakBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// If soil has no water, dries back into dirt
struct SoilDryOutBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Grass transforms dirt into more grass
struct SpreadGrassBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Grass covered by something becomes dirt
struct KillGrassBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// blocks turn adjacent air into itself, up to a certain distance
struct SpreadFoliageBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Blocks fall like sand, but also move left and right randomly.
struct LiquidFlowBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Liquids look down and at diagonals to combine with same blocks
struct LiquidCombineBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Block checks four adjacent blocks for water, turns to obsidian if true
struct LavaToObsidianBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Switches between solid and not solid based on smart updates
struct trapdoorBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};

// Increases sprout growth data item if above soil
struct sproutGrowthBehavior : BlockBehavior
{
    std::vector<DataItemID> Init();
    BehaviorContext UpdateTick(BehaviorContext ctx);
    BehaviorContext RandomTick(BehaviorContext ctx);
    BehaviorContext SmartTick(BehaviorContext ctx);
};