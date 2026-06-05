
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
        RegisterBlock({"air",           -1,  255, 0,   false, false, 15 });
        RegisterBlock({"grass",         320, 0,   0,   true,  true,  15});
        RegisterBlock({"dirt" ,         136, 0,   0,   true,  true,  15});
        RegisterBlock({"stone",         339, 0,   0,   true,  true,  25});
        RegisterBlock({"gold_ore",      303, 0,   75,  true,  true,  25});
        RegisterBlock({"yellow_flower", 205, 255, 255, false, true,  10 });
        RegisterBlock({"twigs",         199, 0,   0,   false, true,  10 });
        RegisterBlock({"water",         2,   0,   0,   false, true,  5 });
    }
}