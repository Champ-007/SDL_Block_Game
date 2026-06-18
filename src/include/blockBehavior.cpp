
#include <iostream>

#include "blockBehavior.hpp"
#include "chunks.hpp"
#include "blockRegistry.hpp"

BehaviorContext::BehaviorContext(int x, int y, BlockID block, BlockData data, Chunk* chunk) :
    x(x),
    y(y),
    block(block),
    data(data),
    chunk(chunk)
    {}

BehaviorContext FallBehavior::OnUpdate(BehaviorContext ctx)
{
    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    BlockID other;
    BlockData otherData;
    int dir = (rand() % 2);
    if (dir == 0) dir = -1;

    // Fall downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x - dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // No movement
    return ctx;

}
BehaviorContext FallBehavior::OnPlace(BehaviorContext ctx)
{
    return ctx;
}
BehaviorContext FallBehavior::OnBreak(BehaviorContext ctx)
{
    return ctx;
}

BehaviorContext SpreadGrassBehavior::OnUpdate(BehaviorContext ctx)
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
                ctx.chunk->SafeSetBlock({offset.x, offset.y}, ctx.block, 0, UpdateType::defered);
            }
        }
    }
    return ctx;
}
BehaviorContext SpreadGrassBehavior::OnPlace(BehaviorContext ctx)
{
    return ctx;
}
BehaviorContext SpreadGrassBehavior::OnBreak(BehaviorContext ctx)
{
    return ctx;
}

BehaviorContext LiquidFlowBehavior::OnUpdate(BehaviorContext ctx)
{
    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    BlockID other;
    BlockData otherData;
    int dir = (rand() % 2);
    if (dir == 0) dir = -1; // 1 or -1

    // Fall downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, ctx.data, blockType);
        return {ctx.x - dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Try moving horizontally toward dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, ctx.data, blockType);
        return {ctx.x + dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // Try to move in opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, ctx.data, blockType);
        return {ctx.x - dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // Determine how to move horizontally
    // if (ctx.data > 1)
    // {
    //     // Fuller water blocks should spread out while flowing

    //     int takeWater  = static_cast<int>(ceil(ctx.data / 2.0f));
    //     int leaveWater = ctx.data - takeWater;

    //     // Try spreading horizontally toward dir
    //     other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
    //     if (BlockRegistry::get(other).name == "air")
    //     {
    //         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, leaveWater, blockType);
    //         ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, takeWater, blockType);

    //         return {ctx.x, ctx.y, ctx.block, static_cast<BlockData>(leaveWater), ctx.chunk};
    //     }

    //     // Try to spread in opposite dir
    //     other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
    //     if (BlockRegistry::get(other).name == "air")
    //     {
    //         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, leaveWater, blockType);
    //         ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, takeWater, blockType);
    //         return {ctx.x, ctx.y, ctx.block, static_cast<BlockData>(leaveWater), ctx.chunk};
    //     }
    // }
    // else if (ctx.data == 1)
    // {
    //     // Shallow water blocks should just move

    //     // Try moving horizontally toward dir
    //     other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
    //     otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
    //     if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    //     {
    //         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
    //         ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, ctx.data, blockType);
    //         return {ctx.x + dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
    //     }

    //     // Try to move in opposite dir
    //     other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
    //     otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
    //     if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    //     {
    //         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
    //         ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, ctx.data, blockType);
    //         return {ctx.x - dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
    //     }
    // }

    // No movement
    return ctx;
}
BehaviorContext LiquidFlowBehavior::OnPlace(BehaviorContext ctx)
{
    return ctx;
}
BehaviorContext LiquidFlowBehavior::OnBreak(BehaviorContext ctx)
{
    return ctx;
}

// BehaviorContext LiquidCombineBehavior::OnUpdate(BehaviorContext ctx)
// {
//     UpdateType otherType = UpdateType::defered;
//     UpdateType blockType = UpdateType::defered;

//     BlockID other;
//     BlockData otherData;
//     int dir = (rand() % 2);
//     if (dir == 0) dir = -1;

//     // Check downwards
//     other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
//     otherData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y + 1});
//     if (other == ctx.block && otherData < 4)
//     {
//         int combineLiquid = static_cast<int>(otherData) + static_cast<int>(ctx.data);

//         if (combineLiquid <= 4) // only enough liquid to fill one block.
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, static_cast<uint8_t>(combineLiquid), blockType);
//             return {ctx.x, ctx.y + 1, ctx.block, static_cast<uint8_t>(combineLiquid), ctx.chunk};
//         }
//         else // both blocks still have liquid
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, static_cast<uint8_t>(combineLiquid - 4), otherType);
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, 4, blockType);
//             return {ctx.x, ctx.y, ctx.block, static_cast<uint8_t>(combineLiquid - 4), ctx.chunk};
//         }
//     }

//     // Check down and to the dir
//     other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
//     otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
//     if (other == ctx.block && otherData < 4)
//     {
//         int combineLiquid = static_cast<int>(otherData) + static_cast<int>(ctx.data);

//         if (combineLiquid <= 4) // only enough liquid to fill one block.
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
//             ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, static_cast<uint8_t>(combineLiquid), blockType);
//             return {ctx.x + dir, ctx.y + 1, ctx.block, static_cast<uint8_t>(combineLiquid), ctx.chunk};
//         }
//         else // both blocks still have liquid
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, static_cast<uint8_t>(combineLiquid - 4), otherType);
//             ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, 4, blockType);
//             return {ctx.x, ctx.y, ctx.block, static_cast<uint8_t>(combineLiquid - 4), ctx.chunk};
//         }
//     }

//     // Check down and to the opposite dir
//     other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
//     otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
//     if (other == ctx.block && otherData < 4)
//     {
//         int combineLiquid = static_cast<int>(otherData) + static_cast<int>(ctx.data);

//         if (combineLiquid <= 4) // only enough liquid to fill one block.
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
//             ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, static_cast<uint8_t>(combineLiquid), blockType);
//             return {ctx.x - dir, ctx.y + 1, ctx.block, static_cast<uint8_t>(combineLiquid), ctx.chunk};
//         }
//         else // both blocks still have liquid
//         {
//             ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, static_cast<uint8_t>(combineLiquid - 4), otherType);
//             ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, 4, blockType);
//             return {ctx.x, ctx.y, ctx.block, static_cast<uint8_t>(combineLiquid - 4), ctx.chunk};
//         }
//     }

//     // Check in dir, level out this time
//     other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
//     otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
//     if (other == ctx.block && abs(otherData - ctx.data) >= 2)
//     {
//         int combineLiquid = static_cast<int>(otherData) + static_cast<int>(ctx.data);
//         int leaveLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
//         int takeLiquid    = combineLiquid - leaveLiquid;

//         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, takeLiquid, otherType);
//         ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, leaveLiquid, blockType);
//         return {ctx.x, ctx.y, ctx.block, static_cast<BlockData>(takeLiquid), ctx.chunk};
//     }

//     // Check in opposite dir, level out
//     other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
//     otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
//     if (other == ctx.block && abs(otherData - ctx.data) >= 2)
//     {
//         int combineLiquid = static_cast<int>(otherData) + static_cast<int>(ctx.data);
//         int leaveLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
//         int takeLiquid    = combineLiquid - leaveLiquid;

//         ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, takeLiquid, otherType);
//         ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, leaveLiquid, blockType);
//         return {ctx.x, ctx.y, ctx.block, static_cast<BlockData>(takeLiquid), ctx.chunk};
//     }

//     // No movement
//     return ctx;
// }
// BehaviorContext LiquidCombineBehavior::OnPlace(BehaviorContext ctx)
// {
//     return ctx;
// }
// BehaviorContext LiquidCombineBehavior::OnBreak(BehaviorContext ctx)
// {
//     return ctx;
// }

BehaviorContext LavaToObsidianBehavior::OnUpdate(BehaviorContext ctx)
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
                ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("obsidian"), 0, UpdateType::defered);
                return {ctx.x, ctx.y, BlockRegistry::getIDByName("obsidian"), ctx.data, ctx.chunk};
            }
        }
    }

    // No change
    return ctx;
}
BehaviorContext LavaToObsidianBehavior::OnPlace(BehaviorContext ctx)
{
    return ctx;
}
BehaviorContext LavaToObsidianBehavior::OnBreak(BehaviorContext ctx)
{
    return ctx;
}
