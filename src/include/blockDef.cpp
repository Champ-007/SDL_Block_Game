
#include "blockDef.hpp"

BlockDef::BlockDef(std::string name, int textureIndex, int skyLight, int blockLight, bool mineable, int mineStrength, bool placeOver, bool isSolid, bool isTransparent, int density, std::vector<std::shared_ptr<BlockBehavior>> behaviors) :
    name(std::move(name)), textureIndex(textureIndex),
    skyLight(skyLight),
    blockLight(blockLight),
    mineable(mineable),
    mineStrength(mineStrength),
    placeOver(placeOver),
    isSolid(isSolid),
    isTransparent(isTransparent),
    density(density),
    behaviors(behaviors)
    {}


// Convenience: dispatch an event to all attached behaviors
void BlockDef::OnPlace(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        b->OnPlace(ctx);
    }
}

void BlockDef::OnBreak(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        b->OnBreak(ctx);
    }
}

void BlockDef::OnUpdate(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        b->OnUpdate(ctx);
    }
}