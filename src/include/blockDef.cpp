
#include "blockDef.hpp"

BlockDef::BlockDef(std::string name, int textureIndex, int skyLight, int blockLight, bool mineable, int mineStrength, bool placeOver, bool isSolid, bool isTransparent, int density, std::vector<std::shared_ptr<BlockBehavior>> behaviors)
{
    this->name = std::move(name);
    this->textureIndex = textureIndex;
    this->skyLight = skyLight;
    this->blockLight = blockLight;
    this->mineable = mineable;
    this->mineStrength = mineStrength;
    this->placeOver = placeOver;
    this->isSolid = isSolid;
    this->isTransparent = isTransparent;
    this->density = density;
    this->behaviors = behaviors;
    // Init the DataItems
    unsigned int bit = 0;
    for (auto b : behaviors)
    {
        // Each behavior sends back a list of the primative dataitems it relies on
        std::vector<DataItemID> dataItemIDs = b->Init();

        // Initialize each dataItem
        for (const auto& dataItemID : dataItemIDs)
        {
            PrimativeDataItem primDataItem = DataItemRegistry::get(dataItemID);
            auto it = std::find(dataItems.begin(), dataItems.end(), primDataItem);
            if (it == dataItems.end())
            {
                // The dataItem does not exist yet
                DataItem newDataItem(primDataItem);
                newDataItem.startingBit = bit;
                dataItems.push_back(newDataItem);
                bit += newDataItem.length;
            }
        }

        // Check for too many bits reserved
        // std::cout << "Debug: sizeof(BlockData) = " << sizeof(BlockData) << std::endl;
        if (bit >= sizeof(BlockData) * 8)
        {
            std::cout << "Error: Memory overlap for " << this->name << " block. Uses " << bit << " bits." << std::endl;
        }
    }
    // std::cout << "Debug: Memory usage for " << this->name << " block: uses " << bit << " bits." << std::endl;
}

bool BlockDef::Find(std::string itemName) const
{
    auto it = std::find(dataItems.begin(), dataItems.end(), itemName);
    if (it != dataItems.end())
    {
        // Found the item
        return true;
    }
    return false;
}

unsigned int BlockDef::Read(std::string itemName, BlockData* data) const
{
    auto it = std::find(dataItems.begin(), dataItems.end(), itemName);
    if (it != dataItems.end())
    {
        // Found the item
        unsigned int result = it->Read(data);
        return result;
    }

    std::cout << "Error: Bad BlockDef DataItem read attempt. name was " << itemName << std::endl;
    return 0;
}

void BlockDef::Write(std::string itemName, BlockData* data, BlockData bits) const
{
    auto it = std::find(dataItems.begin(), dataItems.end(), itemName);
    if (it != dataItems.end())
    {
        // Found the item
        it->Write(data, bits);
        return;
    }

    std::cout << "Error: Bad BlockDef DataItem write attempt. Item was " << itemName << " called on blockDef " << name << std::endl;
}

// Convenience: dispatch an event to all attached behaviors
void BlockDef::UpdateTick(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        ctx = b->UpdateTick(ctx);
    }
}

void BlockDef::RandomTick(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        ctx = b->RandomTick(ctx);
    }
}

void BlockDef::SmartTick(BehaviorContext ctx) const
{
    for (size_t i = 0; i < behaviors.size(); i++)
    {
        auto b = behaviors.at(i);
        ctx = b->SmartTick(ctx);
    }
}