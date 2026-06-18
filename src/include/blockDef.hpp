#pragma once

#include <string>
#include <memory>
#include <vector>

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

    std::vector<std::shared_ptr<BlockBehavior>> behaviors;

    BlockDef(std::string name, int textureIndex, int skyLight, int blockLight, bool mineable, int mineStrength, bool placeOver, bool isSolid, bool isTransparent, int density, bool isLiquid, std::vector<std::shared_ptr<BlockBehavior>> behaviors);

    // Convenience: dispatch an event to all attached behaviors
    void OnPlace(BehaviorContext ctx) const;
    void OnBreak(BehaviorContext ctx) const;
    void OnUpdate(BehaviorContext ctx) const;
};