
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
        for (int i = 0; i < list.size(); i++)
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
        RegisterBlock({"air",           -1,  255, 0,   false, false, 15, true,  true });
        RegisterBlock({"grass",         320, 0,   0,   true,  true,  15, false, false});
        RegisterBlock({"dirt" ,         136, 0,   0,   true,  true,  15, false, false});
        RegisterBlock({"stone",         339, 0,   0,   true,  true,  25, false, false});
        RegisterBlock({"gold_ore",      303, 0,   75,  true,  true,  25, false, false});
        RegisterBlock({"yellow_flower", 205, 255, 255, false, true,  10, false, true });
        RegisterBlock({"twigs",         199, 255, 0,   false, true,  10, false, true });
        RegisterBlock({"water",         2,   0,   0,   false, false, 5,  true,  true });
        RegisterBlock({"lava",          0,   0,   255, false, false, 5,  true,  false});
        RegisterBlock({"gravel",        324, 0,   0,   true,  true,  10, false, false});
    }
}