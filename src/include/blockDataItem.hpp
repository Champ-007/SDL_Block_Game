#pragma once

#include <string>
#include <cstdint>
#include <vector>

using DataItemID = uint16_t; // 65k possible unique dataItems
using BlockData = uint8_t;

struct PrimativeDataItem
{
    std::string name; // Unique identifier for the dataItem
    unsigned int length;       // The number of bits the dataItem latches to

    PrimativeDataItem() = default;
    PrimativeDataItem(std::string name, int length)
    {
        this->name = name;
        this->length = length;
    }

    bool operator==(const PrimativeDataItem& other) const;
};

struct DataItem : PrimativeDataItem
{
    unsigned int startingBit;  // The leftmost bit position that the DataItem latches to

    DataItem() = default;
    DataItem(const PrimativeDataItem& base);

    bool operator==(const PrimativeDataItem& other) const;
    bool operator==(const std::string& other) const;

    unsigned int Read(BlockData* data) const; // Reads the DataItem's bits from the blockData
    void Write(BlockData* data, BlockData bits); // Writes bits to the blockData
};

namespace DataItemRegistry
{
    // Call once at startup to register all unique dataItem types
    void Init();

    // Lookup primitive from ID
    const PrimativeDataItem& get(DataItemID id);

    // Get ID from name
    DataItemID getIDByName(std::string str);

    // Register primative dataItem
    DataItemID RegisterDataItem(PrimativeDataItem dataItem);

}
