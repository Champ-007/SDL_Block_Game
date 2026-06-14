#pragma once

#include <cstdint>

using BlockID = uint16_t;

struct Chunk; // Forward declaration

struct BehaviorContext
{
    int x;
    int y;
    BlockID block;
    Chunk* chunk;
};

// The virtual prototype for all other block behaviors
struct BlockBehavior
{
    virtual ~BlockBehavior() = default;
    virtual void OnUpdate(BehaviorContext ctx) {}
    virtual void OnPlace(BehaviorContext ctx) {}
    virtual void OnBreak(BehaviorContext ctx) {}
};

// Actual behavior declarations

// Classic sand-like behavior. Falls down or at a diagonal
struct FallBehavior : BlockBehavior
{
    void OnUpdate(BehaviorContext ctx);
    void OnPlace(BehaviorContext ctx);
    void OnBreak(BehaviorContext ctx);
};

// Grass transforms dirt into more grass
struct SpreadGrassBehavior : BlockBehavior
{
    void OnUpdate(BehaviorContext ctx);
    void OnPlace(BehaviorContext ctx);
    void OnBreak(BehaviorContext ctx);
};

// Blocks fall like sand, but also move left and right randomly.
struct LiquidFlowBehavior : BlockBehavior
{
    void OnUpdate(BehaviorContext ctx);
    void OnPlace(BehaviorContext ctx);
    void OnBreak(BehaviorContext ctx);
};

struct LavaToObsidianBehavior : BlockBehavior
{
    void OnUpdate(BehaviorContext ctx);
    void OnPlace(BehaviorContext ctx);
    void OnBreak(BehaviorContext ctx);
};
