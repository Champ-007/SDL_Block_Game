#pragma once

#include <string>

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

    BlockDef(std::string name, int textureIndex, int skyLight, int blockLight, bool isSolid, bool mineable, int mineStrength, bool placeOver, bool isTransparent) :
        name(std::move(name)), textureIndex(textureIndex),
        skyLight(skyLight),
        blockLight(blockLight),
        isSolid(isSolid),
        mineable(mineable),
        mineStrength(mineStrength),
        placeOver(placeOver),
        isTransparent(isTransparent)
        {}
};