
#include "structures.hpp"

Structure::Structure(std::string _name, int _width, int _height, std::vector<int> _blocks, std::vector<int> _biomes)
{
    name = _name;
    width = _width;
    height = _height;
    blocks = _blocks;
    biomes = _biomes;
}

namespace StructureRegistry
{
    std::vector<Structure> list;

    // Lookup primitive from ID
    Structure get(StructureID id)
    {
        return list.at(id);
    }
    
    Structure getRandom(int biome, unsigned int rand)
    {
        std::vector<StructureID> structures = {};

        // Loop through every structure
        for (const auto& structure: list)
        {
            // Check if that structure can spawn in our biome
            bool canSpawn = false;
            for (const auto& struct_biome : structure.biomes)
            {
                if (struct_biome == biome)
                {
                    // The structure can spawn
                    canSpawn = true;
                }
            }

            if (canSpawn) structures.push_back(StructureRegistry::getIDByName(structure.name));
        }

        // Pull and return a random structure from the ones that can spawn
        if (!structures.empty())
        {
            unsigned int index = rand % structures.size();
            return get(structures.at(index));
        }
        else
        {
            return Structure();
        }
    }

    // Get ID from name
    StructureID getIDByName(std::string str)
    {
        for (size_t i = 0; i < list.size(); i++)
        {
            if (list.at(i).name == str)
            {
                return i;
            }
        }

        std::cout << "Error: typo structure registry query.\n";
        return 0;
    }

    // Register primative dataItem
    StructureID RegisterStructure(Structure structure)
    {
        StructureID id = static_cast<StructureID>(list.size());
        list.push_back(std::move(structure));
        return id;
    }
    
    // Call once at startup to register all structures
    void Init()
    {
        RegisterStructure({"monolith_1", 5, 5, {
            4, -1, -1, -1,  4, 
            -1, 4,  4,  4, -1, 
            -1, 4,  5,  4, -1, 
            -1, 4,  4,  4, -1, 
            4, -1, -1, -1,  4,
        }, {
            0
        }});
        RegisterStructure({"monolith_2", 5, 5, {
            -1, -1,  4, -1, -1, 
            -1,  4,  4,  4, -1, 
             4,  4,  6,  4,  4, 
            -1,  4,  4,  4, -1, 
            -1, -1,  4, -1, -1,
        }, {
            0
        }});
        RegisterStructure({"monolith_3", 5, 5, {
            -1,  4,  4,  4, -1, 
             4,  4,  9,  4,  4, 
             4,  9,  9,  9,  4, 
             4,  4,  9,  4,  4, 
            -1,  4,  4,  4, -1,
        }, {
            -1, 1
        }});
    }
};