
#include "blockRegistry.hpp"

namespace BlockRegistry
{
    std::vector<BlockDef> list;

    const BlockDef& get(BlockID id)
    {
        return list.at(id);
    }

    BlockID getIDByName(std::string str)
    {
        for (size_t i = 0; i < list.size(); i++)
        {
            if (list.at(i).name == str)
            {
                return i;
            }
        }

        return 0;
    }

    BlockID RegisterBlock(BlockDef block)
    {
        BlockID id = static_cast<BlockID>(list.size());
        list.push_back(std::move(block));
        return id;
    }

    void Init()
    {
        RegisterBlock({"air",           -1,  255, 0,   false, 15, true, false,  true,  0,  false, {

        }});
        RegisterBlock({"grass",         320, 0,   0,   true,  15, false, true,  false, 10, false, {
            std::make_shared<SpreadGrassBehavior>(),
        }});
        RegisterBlock({"dirt" ,         136, 0,   0,   true,  15, false, true,  false, 10, false, {
            
        }});
        RegisterBlock({"stone",         339, 0,   0,   true,  25, false, true,  false, 10, false, {
            
        }});
        RegisterBlock({"obsidian",      12,  0,   0,   true,  75, false, true,  false, 10, false, {
            
        }});
        RegisterBlock({"gold_ore",      303, 0,   75,  true,  25, false, true,  false, 10, false, {
            
        }});
        RegisterBlock({"yellow_flower", 205, 255, 255, true,  10, false, false, true,  2,  false, {
            
        }});
        RegisterBlock({"twigs",         199, 255, 0,   true,  10, false, false, true,  2,  false, {
            
        }});
        RegisterBlock({"water",         2,   0,   0,   false, 5,  true,  false, true,  3,  true,  {
            std::make_shared<LiquidFlowBehavior>(),
            std::make_shared<LiquidCombineBehavior>(),
        }});
        RegisterBlock({"lava",          0,   0,   255, false, 5,  true,  false, true,  6,  true,  {
            std::make_shared<LiquidFlowBehavior>(),
            std::make_shared<LiquidCombineBehavior>(),
            std::make_shared<LavaToObsidianBehavior>(),
        }});
        RegisterBlock({"gravel",        324, 0,   0,   true,  5,  false, true,  false, 8,  false, {
            std::make_shared<FallBehavior>(),
        }});
    }
}