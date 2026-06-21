
#include <iostream>

#include "blockBehavior.hpp"
#include "chunks.hpp"
#include "blockRegistry.hpp"

BehaviorContext::BehaviorContext(int x, int y, BlockID block, BlockData* data, Chunk* chunk) :
    x(x),
    y(y),
    block(block),
    data(data),
    chunk(chunk)
    {}

std::vector<DataItemID> FallBehavior::Init()
{
    return {

    };
}
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
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, *ctx.data, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x, ctx.y + 1});
        return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, *ctx.data, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y + 1});
        return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, *ctx.data, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y + 1});
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

std::vector<DataItemID> SpreadGrassBehavior::Init()
{
    return {
        
    };
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

std::vector<DataItemID> LiquidFlowBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
BehaviorContext LiquidFlowBehavior::OnUpdate(BehaviorContext ctx)
{
    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    BlockID other;
    BlockData otherData;
    int dir = (rand() % 2);
    if (dir == 0) dir = -1; // 1 or -1

    BlockData sourceData = *ctx.data;

    // Fall downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, sourceData, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x, ctx.y + 1});
        return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, sourceData, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y + 1});
        return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Fall down and to the opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
    if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, sourceData, blockType);
        ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y + 1});
        return {ctx.x - dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
    }

    // Determine how to move horizontally
    BlockDef def = BlockRegistry::get(ctx.block);
    unsigned int liquidLevel = def.Read("liquidLevel", ctx.data);
    if (liquidLevel > 0)
    {
        // Fuller water blocks should spread out while flowing

        // 3 = 1 + 1
        // 2 = 1 + 0
        // 1 = 0 + 0

        // Offset into real math space where 1 == 1 unit of water, rather than 0 == 1 unit
        liquidLevel++;

        // Normal calulation
        unsigned int takeWater  = static_cast<int>(floor(static_cast<float>(liquidLevel) / 2.0f));
        unsigned int leaveWater = liquidLevel - takeWater;

        // Offset results back into abstract space
        liquidLevel--;
        takeWater--;
        leaveWater--;

        // std::cout << "Debug: original = " << liquidLevel << " take liquid = " << takeWater << ", leave liquid = " << leaveWater << std::endl;

        // Try spreading horizontally toward dir
        other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
        otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
        if (BlockRegistry::get(other).name == "air")
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, otherData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, sourceData, blockType);
            def.Write("liquidLevel", ctx.data, leaveWater);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y});
            def.Write("liquidLevel", ctx.data, takeWater);
            return {ctx.x + dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }

        // Try to spread in opposite dir
        other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
        otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
        if (BlockRegistry::get(other).name == "air")
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, otherData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, sourceData, blockType);
            def.Write("liquidLevel", ctx.data, leaveWater);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y});
            def.Write("liquidLevel", ctx.data, takeWater);
            return {ctx.x - dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }
    else if (liquidLevel == 0)
    {
        // Shallow water blocks should just move

        // Try moving horizontally toward dir
        other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
        otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
        if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y});
            return {ctx.x + dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }

        // Try to move in opposite dir
        other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
        otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
        if (BlockRegistry::get(other).density < BlockRegistry::get(ctx.block).density)
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, other, otherData, otherType);
            ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y});
            return {ctx.x - dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }

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

std::vector<DataItemID> LiquidCombineBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
BehaviorContext LiquidCombineBehavior::OnUpdate(BehaviorContext ctx)
{
    UpdateType otherType = UpdateType::instant;

    BlockDef def = BlockRegistry::get(ctx.block);
    BlockID other;
    BlockData otherData;
    // BlockData sourceData = *ctx.data;

    unsigned int sourceLiquid = def.Read("liquidLevel", ctx.data) + 1;
    unsigned int otherLiquid;

    int dir = (rand() % 2);
    if (dir == 0) dir = -1;

    // Check downwards
    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y + 1});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && otherLiquid < 4)
    {
        unsigned int combineLiquid = otherLiquid + sourceLiquid; // equal to the amount of water in units.

        if (combineLiquid <= 4) // only enough liquid to fill one block.
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x, ctx.y + 1});
            def.Write("liquidLevel", ctx.data, combineLiquid - 1);
            return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", ctx.data, (combineLiquid - 4) - 1);

            BlockData* otherDataPtr = ctx.chunk->SafeTakeBlockData({ctx.x, ctx.y + 1});
            def.Write("liquidLevel", otherDataPtr, 4 - 1);

            return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }

    // Check down and to the dir
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y + 1});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && otherLiquid < 4)
    {
        unsigned int combineLiquid = otherLiquid + sourceLiquid; // equal to the amount of water in units.

        if (combineLiquid <= 4) // only enough liquid to fill one block.
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y + 1});
            def.Write("liquidLevel", ctx.data, combineLiquid - 1);
            return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", ctx.data, (combineLiquid - 4) - 1);

            BlockData* otherDataPtr = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y + 1});
            def.Write("liquidLevel", otherDataPtr, 4 - 1);

            return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }

    // Check down and to the opposite dir
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y + 1});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y + 1});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && otherLiquid < 4)
    {
        unsigned int combineLiquid = otherLiquid + sourceLiquid; // equal to the amount of water in units.

        if (combineLiquid <= 4) // only enough liquid to fill one block.
        {
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y + 1});
            def.Write("liquidLevel", ctx.data, combineLiquid - 1);
            return {ctx.x - dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", ctx.data, (combineLiquid - 4) - 1);

            BlockData* otherDataPtr = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y + 1});
            def.Write("liquidLevel", otherDataPtr, 4 - 1);

            return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }

    // Check in dir, level out this time
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && abs(sourceLiquid - otherLiquid) >= 2)
    {
        unsigned int combineLiquid = sourceLiquid + otherLiquid;
        unsigned int leaveLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
        unsigned int takeLiquid    = combineLiquid - leaveLiquid;

        def.Write("liquidLevel", ctx.data, takeLiquid - 1);

        BlockData* otherDataPtr = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y});
        def.Write("liquidLevel", otherDataPtr, leaveLiquid - 1);

        return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // Check in opposite dir, level out
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && abs(sourceLiquid - otherLiquid) >= 2)
    {
        unsigned int combineLiquid = sourceLiquid + otherLiquid;
        unsigned int leaveLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
        unsigned int takeLiquid    = combineLiquid - leaveLiquid;

        def.Write("liquidLevel", ctx.data, takeLiquid - 1);

        BlockData* otherDataPtr = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y});
        def.Write("liquidLevel", otherDataPtr, leaveLiquid - 1);

        return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // No movement
    return ctx;
}
BehaviorContext LiquidCombineBehavior::OnPlace(BehaviorContext ctx)
{
    return ctx;
}
BehaviorContext LiquidCombineBehavior::OnBreak(BehaviorContext ctx)
{
    return ctx;
}

std::vector<DataItemID> LavaToObsidianBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
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
                const BlockDef& def = BlockRegistry:: get(ctx.block);
                unsigned int liquidLevel = def.Read("liquidLevel", ctx.data);
                if (liquidLevel == 3)
                {
                    BlockID obsidian = BlockRegistry::getIDByName("obsidian");
                    ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, obsidian, 0, UpdateType::defered);
                    return {ctx.x, ctx.y, obsidian, ctx.data, ctx.chunk};
                }
                else
                {
                    BlockID stone = BlockRegistry::getIDByName("stone");
                    ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, stone, 0, UpdateType::defered);
                    return {ctx.x, ctx.y, stone, ctx.data, ctx.chunk};
                }
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
