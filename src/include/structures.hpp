#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using StructureID = uint16_t; // 65k possible structures

struct Structure
{
    std::string name;
    unsigned int width;
    unsigned int height;
    std::vector<int> blocks;
    std::vector<int> biomes;

    Structure() = default;
    Structure(std::string _name, int _width, int _height, std::vector<int> _blocks, std::vector<int> _biomes);
};

namespace StructureRegistry
{
    // Call once at startup to register all unique dataItem types
    void Init();

    Structure getRandom(int biome, unsigned int rand);

    // Lookup primitive from ID
    Structure get(StructureID id);

    // Get ID from name
    StructureID getIDByName(std::string str);

    // Register primative dataItem
    StructureID RegisterStructure(Structure dataItem);
};