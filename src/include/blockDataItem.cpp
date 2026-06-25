
#include "blockDataItem.hpp"

bool PrimativeDataItem::operator==(const PrimativeDataItem& other) const
{
    return name == other.name;
}

DataItem::DataItem(const PrimativeDataItem& base)
{
    this->name = base.name;
    this->length = base.length;
}

bool DataItem::operator==(const PrimativeDataItem& other) const
{
    return this->name == other.name;
}

bool DataItem::operator==(const std::string& other) const
{
    return this->name == other;
}

unsigned int DataItem::Read(BlockData* data) const
{
    // Shift the data over so that bit 0 is the first bit that the dataItem latches to
    unsigned int shifted = *data >> startingBit;

    // Create a mask of 1s that is length long
    unsigned int mask = (1 << length) - 1;

    // By ANDing these, we collect only length bits, starting at startingBit
    return shifted & mask;
}

void DataItem::Write(BlockData* data, BlockData bits) const
{
    // Create a mask the length of the blockdata, with 1s where we want to write
    BlockData lengthMask;
    unsigned int maskWidth = sizeof(BlockData) * 8;
    if (length >= maskWidth)
    {
        lengthMask = ~BlockData(0); // All 1s, because the length occupies the whole blockData
    }
    else
    {
        lengthMask = (BlockData(1) << length) - 1; // Series of length 1s
    }

    // Move the mask into position
    BlockData positionedMask = lengthMask << startingBit;

    // AND the inverted mask over the data, erasing bits within our mask
    // (our mask now has 0 where we want to write, because 0 & anything is 0, but 1 & anything preserves that data)
    *data = *data & ~positionedMask;

    // Create a new mask the length of the blockdata, with 0s everywhere except for the data where we want to write
    BlockData dataMask = (bits & lengthMask) << startingBit;

    // OR this mask with the data to paste the new data in
    *data = *data | dataMask;
}

namespace DataItemRegistry
{
    std::vector<PrimativeDataItem> list;

    // Lookup primitive from ID
    const PrimativeDataItem& get(DataItemID id)
    {
        return list.at(id);
    }

    // Get ID from name
    DataItemID getIDByName(std::string str)
    {
        for (size_t i = 0; i < list.size(); i++)
        {
            if (list.at(i).name == str)
            {
                return i;
            }
        }

        std::cout << "Error: typo dataItem registry query.\n";
        return 0;
    }

    // Register primative dataItem
    DataItemID RegisterDataItem(PrimativeDataItem dataItem)
    {
        DataItemID id = static_cast<DataItemID>(list.size());
        list.push_back(std::move(dataItem));
        return id;
    }
    
    // Call once at startup to register all unique dataItem types
    void Init()
    {
        RegisterDataItem({"liquidLevel", 2});
        RegisterDataItem({"foliageReach", 2});
        RegisterDataItem({"doorOpen", 1});
    }

}