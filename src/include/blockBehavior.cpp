
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
BehaviorContext FallBehavior::UpdateTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    BlockID other;
    BlockData otherData;
    int dir = (ctx.chunk->GetRand() % 2);
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

    // No change
    return ctx;

}
BehaviorContext FallBehavior::RandomTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext FallBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> DirtSoakBehavior::Init()
{
    return {

    };
}
BehaviorContext DirtSoakBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;

}
BehaviorContext DirtSoakBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    vector2 offsets[3] = {
        {-1, 0},
        {1,  0},
        {0, -1}
    };
    
    for (vector2 offset : offsets)
    {
        offset += {ctx.x, ctx.y};
        BlockID other = ctx.chunk->SafeGetBlock({offset.x, offset.y});
        BlockData otherData = ctx.chunk->SafeGetBlockData({offset.x, offset.y});
        if (other == BlockRegistry::getIDByName("water"))
        {
            const BlockDef& def = BlockRegistry::get(other);
            unsigned int liquidLevel = def.Read("liquidLevel", &otherData);
            if (liquidLevel >= 1)
            {
                BlockID soilID = BlockRegistry::getIDByName("soil");
                ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, soilID, 0, UpdateType::defered);
                return {ctx.x, ctx.y, soilID, ctx.data, ctx.chunk};
            }
        }
    }

    // No change
    return ctx;
}
BehaviorContext DirtSoakBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> SoilDryOutBehavior::Init()
{
    return {

    };
}
BehaviorContext SoilDryOutBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;

}
BehaviorContext SoilDryOutBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    vector2 offsets[3] = {
        {-1, 0},
        {1,  0},
        {0, -1}
    };

    bool foundWater = false;
    
    for (vector2 offset : offsets)
    {
        offset += {ctx.x, ctx.y};
        BlockID other = ctx.chunk->SafeGetBlock({offset.x, offset.y});
        if (other == BlockRegistry::getIDByName("water"))
        {
            foundWater = true;
        }
    }

    if (!foundWater)
    {
        BlockID dirtID = BlockRegistry::getIDByName("dirt");
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, dirtID, 0, UpdateType::defered);
        return {ctx.x, ctx.y, dirtID, ctx.data, ctx.chunk};
    }

    // No change
    return ctx;
}
BehaviorContext SoilDryOutBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> SpreadGrassBehavior::Init()
{
    return {
        
    };
}
BehaviorContext SpreadGrassBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext SpreadGrassBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
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
                break;
            }
        }
    }

    // No change
    return ctx;
}
BehaviorContext SpreadGrassBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> KillGrassBehavior::Init()
{
    return {
        
    };
}
BehaviorContext KillGrassBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext KillGrassBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    BlockID other;

    other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y - 1});
    if (other != BlockRegistry::getIDByName("air"))
    {
        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("dirt"), 0, UpdateType::defered);
    }

    // No change
    return {ctx.x, ctx.y, BlockRegistry::getIDByName("dirt"), 0, ctx.chunk};
}
BehaviorContext KillGrassBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> SpreadFoliageBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("foliageReach")
    };
}
BehaviorContext SpreadFoliageBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext SpreadFoliageBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    BlockID other;
    BlockData sourceData = *ctx.data;
    const BlockDef& def = BlockRegistry::get(ctx.block);
    uint32_t foliageReach = def.Read("foliageReach", &sourceData);

    // Only spread if there is reach left in the foliageReach dataItem, so that growth has a hard limit
    if (foliageReach > 0)
    {
        BlockData otherData = sourceData;
        def.Write("foliageReach", &otherData, uint32_t{foliageReach - 1});
        vector2 offsets[4] = {
            {0, -1},
            {-1, 0},
            {1, 0},
            {0, 1}
        };
    
        for (vector2 offset : offsets)
        {
            offset += {ctx.x, ctx.y};
            other = ctx.chunk->SafeGetBlock({offset.x, offset.y});
            if (other == BlockRegistry::getIDByName("air"))
            {
                ctx.chunk->SafeSetBlock({offset.x, offset.y}, ctx.block, otherData, UpdateType::defered);
                break;
            }
        }
    }


    // No change
    return ctx;
}
BehaviorContext SpreadFoliageBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> LiquidFlowBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
BehaviorContext LiquidFlowBehavior::UpdateTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;

    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    BlockID other;
    BlockData otherData;
    int dir = (ctx.chunk->GetRand() % 2);
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
    const BlockDef& def = BlockRegistry::get(ctx.block);
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
            def.Write("liquidLevel", &otherData, leaveWater);
            def.Write("liquidLevel", &sourceData, takeWater);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, otherData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y});
            return {ctx.x + dir, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }

        // Try to spread in opposite dir
        other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
        otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
        if (BlockRegistry::get(other).name == "air")
        {
            def.Write("liquidLevel", &otherData, leaveWater);
            def.Write("liquidLevel", &sourceData, takeWater);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, otherData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y});
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

    // No change
    return ctx;
}
BehaviorContext LiquidFlowBehavior::RandomTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext LiquidFlowBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> LiquidCombineBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
BehaviorContext LiquidCombineBehavior::UpdateTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    UpdateType otherType = UpdateType::defered;
    UpdateType blockType = UpdateType::defered;

    const BlockDef& def = BlockRegistry::get(ctx.block);
    BlockID other;
    BlockData otherData;
    BlockData sourceData = *ctx.data;
    // BlockData sourceData = *ctx.data;

    unsigned int sourceLiquid = def.Read("liquidLevel", ctx.data) + 1;
    unsigned int otherLiquid;

    int dir = (ctx.chunk->GetRand() % 2);
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
            def.Write("liquidLevel", &sourceData, combineLiquid - 1);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x, ctx.y + 1});
            return {ctx.x, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", &sourceData, (combineLiquid - 4) - 1);
            def.Write("liquidLevel", &otherData, 4 - 1);

            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y + 1}, ctx.block, otherData, blockType);

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
            def.Write("liquidLevel", &sourceData, combineLiquid - 1);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x + dir, ctx.y + 1});
            return {ctx.x + dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", &sourceData, (combineLiquid - 4) - 1);
            def.Write("liquidLevel", &otherData, 4 - 1);

            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, otherData, blockType);

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
            def.Write("liquidLevel", &sourceData, combineLiquid - 1);
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, BlockRegistry::getIDByName("air"), 0, otherType);
            ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y + 1}, ctx.block, sourceData, blockType);
            ctx.data = ctx.chunk->SafeTakeBlockData({ctx.x - dir, ctx.y + 1});
            return {ctx.x - dir, ctx.y + 1, ctx.block, ctx.data, ctx.chunk};
        }
        else // both blocks still have liquid
        {
            def.Write("liquidLevel", &sourceData, (combineLiquid - 4) - 1);
            def.Write("liquidLevel", &otherData, 4 - 1);

            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, blockType);
            ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y + 1}, ctx.block, otherData, blockType);

            return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
        }
    }

    // Check in dir, level out this time
    other = ctx.chunk->SafeGetBlock({ctx.x + dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x + dir, ctx.y});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && abs(sourceLiquid - otherLiquid) >= 1)
    {
        unsigned int combineLiquid = sourceLiquid + otherLiquid;
        unsigned int sendLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
        unsigned int keepLiquid    = combineLiquid - sendLiquid;

        def.Write("liquidLevel", &sourceData, keepLiquid - 1);
        def.Write("liquidLevel", &otherData, sendLiquid - 1);

        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, blockType);
        ctx.chunk->SafeSetBlock({ctx.x + dir, ctx.y}, ctx.block, otherData, blockType);

        return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // Check in opposite dir, level out
    other = ctx.chunk->SafeGetBlock({ctx.x - dir, ctx.y});
    otherData = ctx.chunk->SafeGetBlockData({ctx.x - dir, ctx.y});
    otherLiquid = def.Read("liquidLevel", &otherData) + 1;
    if (other == ctx.block && abs(sourceLiquid - otherLiquid) >= 2)
    {
        unsigned int combineLiquid = sourceLiquid + otherLiquid;
        unsigned int sendLiquid   = static_cast<int>(ceil(combineLiquid / 2.0f));
        unsigned int keepLiquid    = combineLiquid - sendLiquid;

        def.Write("liquidLevel", &sourceData, keepLiquid - 1);
        def.Write("liquidLevel", &otherData, sendLiquid - 1);

        ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, blockType);
        ctx.chunk->SafeSetBlock({ctx.x - dir, ctx.y}, ctx.block, otherData, blockType);

        return {ctx.x, ctx.y, ctx.block, ctx.data, ctx.chunk};
    }

    // No change
    return ctx;
}
BehaviorContext LiquidCombineBehavior::RandomTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext LiquidCombineBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> LavaToObsidianBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("liquidLevel")
    };
}
BehaviorContext LavaToObsidianBehavior::UpdateTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
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
BehaviorContext LavaToObsidianBehavior::RandomTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext LavaToObsidianBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}

std::vector<DataItemID> trapdoorBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("doorOpen")
    };
}
BehaviorContext trapdoorBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext trapdoorBehavior::RandomTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
BehaviorContext trapdoorBehavior::SmartTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;

    BlockData sourceData = *ctx.data;
    const BlockDef& def = BlockRegistry::get(ctx.block);
    bool doorOpen = static_cast<bool>(def.Read("doorOpen", &sourceData));
    doorOpen = !doorOpen;
    def.Write("doorOpen", &sourceData, doorOpen);
    ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, UpdateType::defered);

    // No change
    return ctx;
}

std::vector<DataItemID> sproutGrowthBehavior::Init()
{
    return {
        DataItemRegistry::getIDByName("sproutGrowth"),
    };
}
BehaviorContext sproutGrowthBehavior::UpdateTick(BehaviorContext ctx)
{
    // No change
    return ctx;

}
BehaviorContext sproutGrowthBehavior::RandomTick(BehaviorContext ctx)
{
    // Basic corruption catch
    if (ctx.data == nullptr) return ctx;
    
    BlockID other = ctx.chunk->SafeGetBlock({ctx.x, ctx.y + 1});
    if (other == BlockRegistry::getIDByName("soil"))
    {
        const BlockDef& def = BlockRegistry::get(ctx.block);
        unsigned int growth = def.Read("sproutGrowth", ctx.data);
        if (growth < 7)
        {
            // increase data item
            BlockData sourceData = ctx.chunk->SafeGetBlockData({ctx.x, ctx.y});
            def.Write("sproutGrowth", &sourceData, static_cast<BlockData>(growth + 1));
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y}, ctx.block, sourceData, UpdateType::defered);
            return ctx;
        }
        else if (growth == 7)
        {
            // Spread upwards
            ctx.chunk->SafeSetBlock({ctx.x, ctx.y - 1}, BlockRegistry::getIDByName("wheat"), 0, UpdateType::defered);
            return ctx;
        }
    }

    // No change
    return ctx;
}
BehaviorContext sproutGrowthBehavior::SmartTick(BehaviorContext ctx)
{
    // No change
    return ctx;
}
