
#include <iostream>

#include "blockBehavior.hpp"
#include "chunks.hpp"
#include "blockRegistry.hpp"

void FallBehavior::OnUpdate(BehaviorContext ctx)
{
    BlockID other;
    int dir = (rand() % 2);
    if (dir == 0) dir = -1;

    // Fall downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block);
        ctx.chunk->AddToBlockIgnore(ctx.x, ctx.y + 1);
    }
    else
    {
        // Fall down and to the dir
        other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
        if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block);
            ctx.chunk->AddToBlockIgnore(ctx.x + dir, ctx.y + 1);
        }
        else
        {
            // Fall down and to the opposite dir
            other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
            if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
            {
                ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
                ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block);
                ctx.chunk->AddToBlockIgnore(ctx.x - dir, ctx.y + 1);
            }
        }
    }

}
void FallBehavior::OnPlace(BehaviorContext ctx)
{

}
void FallBehavior::OnBreak(BehaviorContext ctx)
{

}

void SpreadGrassBehavior::OnUpdate(BehaviorContext ctx)
{
    // std::cout << "Debug: grass update attempted." << std::endl;
    BlockID other;

    vector2 offsets[6] = {
        {-1, -1},
        {-1, 0 },
        {-1, 1 },
        {1,  -1},
        {1,  0 },
        {1,  1 }
    };

    for (vector2 offset : offsets)
    {
        offset += {ctx.x, ctx.y};
        other = ctx.chunk->SafeGetBlock({offset.x, offset.y});
        if (other == BlockRegistry::getIDByName("dirt"))
        {
            if (ctx.chunk->SafeGetBlock({offset.x, offset.y - 1}) == BlockRegistry::getIDByName("air"))
            {
                ctx.chunk->SafeSetBlock({offset.x, offset.y}, ctx.block);
                ctx.chunk->AddToBlockIgnore(offset.x, offset.y);
            }
        }
    }

}
void SpreadGrassBehavior::OnPlace(BehaviorContext ctx)
{

}
void SpreadGrassBehavior::OnBreak(BehaviorContext ctx)
{

}

void LiquidFlowBehavior::OnUpdate(BehaviorContext ctx)
{
    BlockID other;
    int dir = (rand() % 2);
    if (dir == 0) dir = -1;
    // std::cout << "Debug: dir = " << dir << std::endl;

    // Fall downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block);
        ctx.chunk->AddToBlockIgnore(ctx.x, ctx.y + 1);
    }
    else
    {
        // Fall down and to the dir
        other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
        if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block);
            ctx.chunk->AddToBlockIgnore(ctx.x + dir, ctx.y + 1);
        }
        else
        {
            // Fall down and to the opposite dir
            other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
            if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
            {
                ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
                ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block);
                ctx.chunk->AddToBlockIgnore(ctx.x - dir, ctx.y + 1);
            }
            else
            {
                // Try moving horizontally
                other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
                if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
                {
                    ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other);
                    ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block);
                    ctx.chunk->AddToBlockIgnore(ctx.x + dir, ctx.y);
                }
            }
        }
    }

}
void LiquidFlowBehavior::OnPlace(BehaviorContext ctx)
{

}
void LiquidFlowBehavior::OnBreak(BehaviorContext ctx)
{

}

void LavaToObsidianBehavior::OnUpdate(BehaviorContext ctx)
{
    BlockID other;
    vector2 offsets[4] = {
        {-1, 0},
        {1,  0},
        {0, -1},
        {0,  1}
    };

    if (ctx.chunk->SafeGetBlock({ctx.x, ctx.y}) == BlockRegistry::getIDByName("lava"))
    {
        for (vector2 offset : offsets)
        {
            offset += {ctx.x, ctx.y};
            other = ctx.chunk->SafeGetBlock({offset.x, offset.y});
            if (other == BlockRegistry::getIDByName("water"))
            {
                ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("obsidian"));
                ctx.chunk->AddToBlockIgnore(ctx.x, ctx.y);
                return;
            }
        }
    }
}
void LavaToObsidianBehavior::OnPlace(BehaviorContext ctx)
{

}
void LavaToObsidianBehavior::OnBreak(BehaviorContext ctx)
{

}
