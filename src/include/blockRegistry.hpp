#pragma once

#include <vector>
#include <cstdint>

#include "blockDef.hpp"

using BlockID = uint16_t; // 65k unique block types

namespace BlockRegistry
{
    // Call once at startup to register all block definitions
    void Init();

    size_t size();

    // Lookup definition from ID
    const BlockDef& get(BlockID id);

    // Use name to get ID
    BlockID getIDByName(std::string str);

    // Register block definitions to the registry
    BlockID RegisterBlock(BlockDef block);

}