#pragma once

#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include "blockBehavior.hpp" 

struct BlockDef
{
    std::string name;
    int  textureIndex;
    int  skyLight;
    int  blockLight;
    bool mineable;
    int  mineStrength;
    bool placeOver;
    bool isSolid;
    bool isTransparent;
    int  density;
    bool isLiquid;

    std::vector<DataItem> dataItems;

    std::vector<std::shared_ptr<BlockBehavior>> behaviors;

    BlockDef(std::string name, int textureIndex, int skyLight, int blockLight, bool mineable, int mineStrength, bool placeOver, bool isSolid, bool isTransparent, int density, bool isLiquid, std::vector<std::shared_ptr<BlockBehavior>> behaviors);

    unsigned int Read(std::string itemName, BlockData* data) const;
    void Write(std::string itemName, BlockData* data, BlockData bits);

    // Dispatch an event to all attached behaviors
    void UpdateTick(BehaviorContext ctx) const;
    void RandomTick(BehaviorContext ctx) const;
};