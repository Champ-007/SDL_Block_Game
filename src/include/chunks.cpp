
#include "chunks.hpp"

Chunk::Chunk(ChunkCoord c, int seed, ChunkEngine* _master)
{
    blocks.resize(CHUNK_LENGTH);
    blockData.resize(CHUNK_LENGTH);
    sky_lights.resize(CHUNK_LENGTH);
    block_lights.resize(CHUNK_LENGTH);
    inLightQueue.resize(CHUNK_LENGTH);
    pos = c;
    master = _master;
    biome = 0;
    Generate(c, seed);
    updateTickTimeTotal = 0.0f;
    updateTickTimeLimit = 128.0f;
    randomTickTimeTotal = 0.0f;
    randomTickTimeLimit = 96.0f;
}

uint32_t Chunk::GetRand()
{
    return master->GetRand();
}

int Chunk::GetBiome()
{
    return biome;
}

ChunkCoord Chunk::GetPosition()
{
    return pos;
}

void Chunk::SetPosition(ChunkCoord c)
{
    pos = c;
}

int Chunk::GetLightQueLength()
{
    return LightUpdateQueue.size();
}

std::pair<int, int> Chunk::GetLight(int i)
{
    std::pair<int, int> result;
    result.first = sky_lights[i];
    result.second = block_lights[i];
    return result;
}

std::pair<int, int> Chunk::TryGetLight(int index, vector2 offset)
{
    vector2 sample_block_pos = {
        floor(index % CHUNK_WIDTH) + offset.x,
        floor(index / CHUNK_WIDTH) + offset.y
    };

    std::pair<int, int> result;

    if (sample_block_pos.x >= 0 && sample_block_pos.x < CHUNK_WIDTH && sample_block_pos.y >= 0 && sample_block_pos.y < CHUNK_WIDTH)
    {
        result.first = sky_lights[index + offset.x + (offset.y * CHUNK_WIDTH)];
        result.second = block_lights[index + offset.x + (offset.y * CHUNK_WIDTH)];
        return result;
    }
    else
    {
        vector2 world_block_pos = {
            sample_block_pos.x + (pos.x * CHUNK_WIDTH),
            sample_block_pos.y + (pos.y * CHUNK_WIDTH)
        };

        return master->GetLight(world_block_pos);
        // return 0;
    }

}

int Chunk::GetSkyLight(int i)
{
    return sky_lights[i];
}

int Chunk::TryGetSkyLight(int index, vector2 offset)
{
    vector2 sample_block_pos = {
        floor(index % CHUNK_WIDTH) + offset.x,
        floor(index / CHUNK_WIDTH) + offset.y
    };

    if (sample_block_pos.x >= 0 && sample_block_pos.x < CHUNK_WIDTH && sample_block_pos.y >= 0 && sample_block_pos.y < CHUNK_WIDTH)
    {
        return sky_lights[index + offset.x + (offset.y * CHUNK_WIDTH)];
    }
    else
    {
        vector2 world_block_pos = {
            sample_block_pos.x + (pos.x * CHUNK_WIDTH),
            sample_block_pos.y + (pos.y * CHUNK_WIDTH)
        };
        return master->GetSkyLight(world_block_pos);
        // return 0;
    }
}

int Chunk::GetBlockLight(int i)
{
    return block_lights[i];
}

int Chunk::TryGetBlockLight(int index, vector2 offset)
{
    vector2 sample_block_pos = {
        floor(index % CHUNK_WIDTH) + offset.x,
        floor(index / CHUNK_WIDTH) + offset.y
    };

    if (sample_block_pos.x >= 0 && sample_block_pos.x < CHUNK_WIDTH && sample_block_pos.y >= 0 && sample_block_pos.y < CHUNK_WIDTH)
    {
        return block_lights[index + offset.x + (offset.y * CHUNK_WIDTH)];
    }
    else
    {
        vector2 world_block_pos = {
            sample_block_pos.x + (pos.x * CHUNK_WIDTH),
            sample_block_pos.y + (pos.y * CHUNK_WIDTH)
        };
        return master->GetBlockLight(world_block_pos);
        // return 0;
    }
}

void Chunk::AddToLightQueue(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH)
    {
        if (!inLightQueue[x + (y * CHUNK_WIDTH)])
        {
            // std::cout << "Debug: adding queue at " << x + (y * CHUNK_WIDTH) << std::endl;
            LightUpdateQueue.push(x + (y * CHUNK_WIDTH));
            inLightQueue[x + (y * CHUNK_WIDTH)] = true;
        }
    }
    // else std::cout << "Error: bad queue attempt." << std::endl;
}

void Chunk::QueueBlockUpdate(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH)
    {
        // std::cout << "Debug: instant queue" << std::endl;
        int index = x + (y * CHUNK_WIDTH);
        bool already = false;
        for (int i : BlockUpdateQueue)
        {
            if (i == index) already = true;
        }
        if (!already)
        {
            BlockUpdateQueue.push_back(x + (y * CHUNK_WIDTH));
        }
    }
    // else std::cout << "Error: bad instant queue attempt." << std::endl;
}

void Chunk::DeferBlockUpdate(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH)
    {
        // std::cout << "Debug: defered queue" << std::endl;
        int index = x + (y * CHUNK_WIDTH);
        bool already = false;
        for (int i : BlockUpdateDefer)
        {
            if (i == index) already = true;
        }
        if (!already)
        {
            BlockUpdateDefer.push_back(x + (y * CHUNK_WIDTH));
        }
    }
    // else std::cout << "Error: bad defered queue attempt." << std::endl;
}

void Chunk::SoftDeferBlockUpdate(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH)
    {
        // std::cout << "Debug: soft defered queue" << std::endl;
        int index = x + (y * CHUNK_WIDTH);
        bool already = false;
        for (int i : BlockUpdateQueue)
        {
            if (i == index) already = true;
        }
        if (!already)
        {
            BlockUpdateDefer.push_back(x + (y * CHUNK_WIDTH));
        }
    }
    // else std::cout << "Error: bad soft queue attempt. x = " << x << ", y = " << y << std::endl;
}

BlockID Chunk::GetBlock(int x, int y)
{
    return blocks[x + (y * CHUNK_WIDTH)];
}

BlockID Chunk::GetBlock(int i)
{
    return blocks[i];
}

void Chunk::SetBlock(int x, int y, BlockID block)
{
    blocks[x + (y * CHUNK_WIDTH)] = block;
}

BlockData Chunk::GetBlockData(int i)
{
    return blockData[i];
}

BlockData Chunk::GetBlockData(int x, int y)
{
    return blockData[x + (y * CHUNK_WIDTH)];
}

void Chunk::SetBlockData(int x, int y, BlockData data)
{
    blockData[x + (y * CHUNK_WIDTH)] = data;
}

BlockID Chunk::SafeGetBlock(vector2 pos_block)
{
    if (pos_block.x >= 0 && pos_block.x < CHUNK_WIDTH && pos_block.y >= 0 && pos_block.y < CHUNK_WIDTH)
    {
        return blocks[pos_block.x + (pos_block.y * CHUNK_WIDTH)];
    }
    else
    {
        vector2 world_block_pos = {
            pos_block.x + (pos.x * CHUNK_WIDTH),
            pos_block.y + (pos.y * CHUNK_WIDTH)
        };

        return master->GetBlock(world_block_pos);
    }
}

void Chunk::SafeSetBlock(vector2 pos_block, BlockID block, BlockData data, UpdateType updateType)
{
    if (pos_block.x >= 0 && pos_block.x < CHUNK_WIDTH && pos_block.y >= 0 && pos_block.y < CHUNK_WIDTH)
    {
        blocks[pos_block.x + (pos_block.y * CHUNK_WIDTH)] = block;
        blockData[pos_block.x + (pos_block.y * CHUNK_WIDTH)] = data;
        AddToLightQueue(pos_block.x, pos_block.y);

        // Queue the block update and the four adjacent blocks
        switch (updateType)
        {
            case UpdateType::instant:
                QueueBlockUpdate(pos_block.x, pos_block.y);
                break;
            case UpdateType::defered:
                DeferBlockUpdate(pos_block.x, pos_block.y);
                break;
            case UpdateType::softDefered:
                SoftDeferBlockUpdate(pos_block.x, pos_block.y);
                break;
        }

        SoftDeferBlockUpdate(pos_block.x + 1, pos_block.y); // right
        SoftDeferBlockUpdate(pos_block.x - 1, pos_block.y); // left
        SoftDeferBlockUpdate(pos_block.x, pos_block.y + 1); // down
        SoftDeferBlockUpdate(pos_block.x, pos_block.y - 1); // up

        vector2 chunk_pos_block = {
            pos.x * CHUNK_WIDTH,
            pos.y * CHUNK_WIDTH
        };
        master->AddBlockToChunkSave(chunk_pos_block, pos_block, block, data);
    }
    else
    {
        vector2 world_block_pos = {
            pos_block.x + (pos.x * CHUNK_WIDTH),
            pos_block.y + (pos.y * CHUNK_WIDTH)
        };

        master->SetBlock(world_block_pos, block, data, updateType);
    }
}

BlockData Chunk:: SafeGetBlockData(vector2 pos_block)
{
    if (pos_block.x >= 0 && pos_block.x < CHUNK_WIDTH && pos_block.y >= 0 && pos_block.y < CHUNK_WIDTH)
    {
        return blockData[pos_block.x + (pos_block.y * CHUNK_WIDTH)];
    }
    else
    {
        vector2 world_block_pos = {
            pos_block.x + (pos.x * CHUNK_WIDTH),
            pos_block.y + (pos.y * CHUNK_WIDTH)
        };

        return master->GetBlockData(world_block_pos);
    }
}

BlockData* Chunk::TakeBlockData(vector2 pos_block)
{
    return &blockData[pos_block.x + (pos_block.y * CHUNK_WIDTH)];
}

BlockData* Chunk::SafeTakeBlockData(vector2 pos_block)
{
    if (pos_block.x >= 0 && pos_block.x < CHUNK_WIDTH && pos_block.y >= 0 && pos_block.y < CHUNK_WIDTH)
    {
        return &blockData[pos_block.x + (pos_block.y * CHUNK_WIDTH)];
    }
    else
    {
        vector2 world_block_pos = {
            pos_block.x + (pos.x * CHUNK_WIDTH),
            pos_block.y + (pos.y * CHUNK_WIDTH)
        };

        return master->TakeBlockData(world_block_pos);
    }
}

void Chunk::Generate(ChunkCoord c, int seed)
{
    // mersenne twister for blockdata
    std::mt19937 randData;
    ChunkHasher hasher;
    randData.seed(hasher(c));

    // noise object for general terrain density
    FastNoiseLite terrain_noise;
    terrain_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrain_noise.SetFrequency(0.06f);
    terrain_noise.SetSeed(seed);
    
    // Noise object for biome
    FastNoiseLite biome_noise;
    biome_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biome_noise.SetFrequency(0.2f);
    biome_noise.SetSeed(seed + 1);
    
    // Noise object for basic terrain details
    FastNoiseLite details_noise;
    details_noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    details_noise.SetFrequency(0.35f);
    details_noise.SetSeed(seed + 1);

    float biome_heat = biome_noise.GetNoise(floor(c.x), floor(c.y));
    if (biome_heat < -0.5) biome = -1;
    if (biome_heat > 0.5) biome = 1;

    // Noise Gen
    for (int y = 0; y < CHUNK_WIDTH; y++)
    {
        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            int index_x = x + (c.x * CHUNK_WIDTH);
            int index_y = y + (c.y * CHUNK_WIDTH);
            float density  = terrain_noise.GetNoise((float)index_x, (float)index_y);
            float density2 = details_noise.GetNoise((float)index_x, (float)index_y);
            int i = x + (y * CHUNK_WIDTH);
            sky_lights[i] = 0;
            block_lights[i] = 0;
            blockData[i] = 0;

            if (density < -0.95f)
            {
                blocks[i] = BlockRegistry::getIDByName("trapdoor");
            }
            else if (density < 0.0f)
            {
                if (terrain_noise.GetNoise((float)index_x, (float)index_y + 1) > 0.0f)
                {
                    if (density2 < -0.4)
                    {
                        blocks[i] = BlockRegistry::getIDByName("yellow_flower");
                        blockData[i] = randData();
                    }
                    else if (density2 > 0.4)
                    {
                        blocks[i] = BlockRegistry::getIDByName("twigs");
                        blockData[i] = randData();
                    }
                    else if (density2 > -0.2 && density2 < 0.2)
                    {
                        blocks[i] = BlockRegistry::getIDByName("wheat");
                    }
                }
                else
                {
                    float limit = 0.6;
                    if (density2 < -limit)
                    {
                        blocks[i] = BlockRegistry::getIDByName("water");
                        blockData[i] = randData();
                    }
                    else if (density2 > limit)
                    {
                        blocks[i] = BlockRegistry::getIDByName("lava");
                        blockData[i] = randData();
                    }
                    else
                    blocks[i] = BlockRegistry::getIDByName("air");
                }
            }
            else if (density < 0.4 && biome == 0)
            {
                if (terrain_noise.GetNoise((float)index_x, ((float)index_y - 1)) < 0.0f)
                {
                    blocks[i] = BlockRegistry::getIDByName("grass");
                }
                else
                {
                    blocks[i] = BlockRegistry::getIDByName("dirt");
                }
            }
            else
            {
                if (density2 < -0.4f)
                {
                    blocks[i] = BlockRegistry::getIDByName("gold_ore");
                }
                else if (density2 > 0.4)
                {
                    blocks[i] = BlockRegistry::getIDByName("gravel");
                }
                else
                {
                    blocks[i] = BlockRegistry::getIDByName("stone");
                }
            }
            inLightQueue[i] = true;
            LightUpdateQueue.push(i);
        }
    }

    // Structure Gen
    bool shouldSpawn = randData() % 2 == 0;
    shouldSpawn = true;
    if (shouldSpawn)
    {
        // Generate a structure within this chunk

        // Get a random structure
        const Structure structure = StructureRegistry::getRandom(biome, randData());
        // std::cout << "Debug: structure name is " << structure.name << "\n";
        unsigned int x = randData() % (CHUNK_WIDTH - structure.width);
        unsigned int y = randData() % (CHUNK_WIDTH - structure.height);
        unsigned int offset_x = 0;
        unsigned int offset_y = 0;

        for (const auto& block : structure.blocks)
        {
            if (block != -1)
            {
                SetBlock(x + offset_x, y + offset_y, block);
            }

            offset_x += 1;
            if (offset_x >= structure.width)
            {
                offset_x = 0;
                offset_y += 1;
            }
        }
    }
    
}

void Chunk::GenerateSave(ChunkSave* save)
{
    for (size_t i = 0; i < save->positions.size(); i++)
    {
        // std::cout << "Debug: setting block to " << save->blocks.at(i) << std::endl; 
        blocks.at(save->positions.at(i)) = save->blocks.at(i);
        blockData.at(save->positions.at(i)) = static_cast<int>(save->data.at(i));
        // if (save->blocks.at(i) < BlockRegistry::size())
        // {
        // }
        // std::cout << "Debug: blockData was " << static_cast<int>(save->data.at(i)) << std::endl;
    }
    
}

void Chunk::UpdateTick()
{
    // int budget = 200;
    while (!BlockUpdateQueue.empty())
    {
        // Get the update location
        int index = BlockUpdateQueue.front();
        BlockUpdateQueue.pop_front();
        // budget--;
        
        // Check if the block update should be ignored
        bool ignore = false;
        for (int i : BlockUpdateDefer)
        {
            if (index == i) ignore = true;
        }

        if (!ignore)
        {
            BlockID block = blocks[index];
            BlockData* data = &blockData[index];
            BlockDef def = BlockRegistry::get(block);
    
            // Create the context and run the OnUpdate() function
            BehaviorContext ctx;
            ctx.block = block;
            ctx.data = data;
            ctx.chunk = this;
            ctx.x = floor(index % CHUNK_WIDTH);
            ctx.y = floor(index / CHUNK_WIDTH);
    
            def.UpdateTick(ctx);

        }
    }
    BlockUpdateQueue.splice(BlockUpdateQueue.end(), BlockUpdateDefer);
    BlockUpdateDefer.clear();
}

void Chunk::RandomTick()
{
    unsigned int index = GetRand() % CHUNK_LENGTH;
    if (index >= CHUNK_LENGTH || index < 0)
    {
        std::cout << "Error: bad random tick index: " << index << std::endl;
        return;
    }

    BlockID block = blocks.at(index);
    BlockData* data = &blockData.at(index);
    BlockDef def = BlockRegistry::get(block);

    // Create the context for the RandomTick function
    BehaviorContext ctx;
    ctx.block = block;
    ctx.data = data;
    ctx.chunk = this;
    ctx.x = floor(index % CHUNK_WIDTH);
    ctx.y = floor(index / CHUNK_WIDTH);

    def.RandomTick(ctx);
}

void Chunk::UpdateLight()
{
    // std::cout << "Debug: update light" << std::endl;
    int budget = 300;
    while (!LightUpdateQueue.empty() && budget > 0) 
    {
        int block_highest = 0;
        int sky_highest = 0;
        int index = LightUpdateQueue.front();
        vector2 pos = {
            index % CHUNK_WIDTH,
            floor(index / CHUNK_WIDTH)
        };
        int block_type = blocks[index];
        BlockDef def = BlockRegistry::get(block_type);
        int sky_light_tran = sky_lights[index];
        int block_light_tran = block_lights[index];
        
        // Get the highest surrounding light for block
        int sky_light;
        int block_light;
        std::pair<int, int> light;

        // To the right
        light = TryGetLight(index, {1, 0});
        sky_light = light.first;
        block_light = light.second;
        if (sky_light > sky_highest) sky_highest = sky_light;
        if (block_light > block_highest) block_highest = block_light;

        // To the left
        light = TryGetLight(index, {-1, 0});
        sky_light = light.first;
        block_light = light.second;
        if (sky_light > sky_highest) sky_highest = sky_light;
        if (block_light > block_highest) block_highest = block_light;

        // To the up
        light = TryGetLight(index, {0, -1});
        sky_light = light.first;
        block_light = light.second;
        if (sky_light > sky_highest) sky_highest = sky_light;
        if (block_light > block_highest) block_highest = block_light;

        // To the down
        light = TryGetLight(index, {0, 1});
        sky_light = light.first;
        block_light = light.second;
        if (sky_light > sky_highest) sky_highest = sky_light;
        if (block_light > block_highest) block_highest = block_light; 

        // raise light of block to either -1 of highest adjacent light, or native light of block, whichever is higher
        int sky_native = def.skyLight;
        if (biome != 0) sky_native = 0;
        int block_native = def.blockLight;
        sky_lights[index] = std::max({sky_native, sky_highest - LIGHT_DECAY_SKY, 0});
        block_lights[index] = std::max({block_native, block_highest - LIGHT_DECAY_BLOCK, 0});
        
        // Propogate the update
        if (sky_lights[index] != sky_light_tran || block_lights[index] != block_light_tran)
        {
            // To the right
            if (index % CHUNK_WIDTH != CHUNK_WIDTH - 1)
            {
                if (!inLightQueue[index + 1])
                {
                    LightUpdateQueue.push(index + 1);
                    inLightQueue[index + 1] = true;
                }
            }
            else
            {
                // Tell our master engine to queue the next block over
                vector2 block_pos;
                block_pos.x = floor(index % CHUNK_WIDTH) + (pos.x * CHUNK_WIDTH) + 1;
                block_pos.y = floor(index / CHUNK_WIDTH) + (pos.y * CHUNK_WIDTH);
                master->QueueBlock(block_pos);
            }
            // To the left
            if (index % CHUNK_WIDTH != 0)
            {
                if (!inLightQueue[index - 1])
                {
                    LightUpdateQueue.push(index - 1);
                    inLightQueue[index - 1] = true;
                    // anything
                }
            }
            else
            {
                // Tell our master engine to queue the next block over
                vector2 block_pos;
                block_pos.x = floor(index % CHUNK_WIDTH) + (pos.x * CHUNK_WIDTH) - 1;
                block_pos.y = floor(index / CHUNK_WIDTH) + (pos.y * CHUNK_WIDTH);
                master->QueueBlock(block_pos);
            }
            // To the down
            if (index < CHUNK_LENGTH - CHUNK_WIDTH)
            {
                if (!inLightQueue[index + CHUNK_WIDTH])
                {
                    LightUpdateQueue.push(index + CHUNK_WIDTH);
                    inLightQueue[index + CHUNK_WIDTH] = true;
                }
            }
            else
            {
                // Tell our master engine to queue the next block over
                vector2 block_pos;
                block_pos.x = floor(index % CHUNK_WIDTH) + (pos.x * CHUNK_WIDTH);
                block_pos.y = floor(index / CHUNK_WIDTH) + (pos.y * CHUNK_WIDTH) + 1;
                master->QueueBlock(block_pos);
            }
            // To the up
            if (index >= CHUNK_WIDTH)
            {
                if (!inLightQueue[index - CHUNK_WIDTH])
                {
                    LightUpdateQueue.push(index - CHUNK_WIDTH);
                    inLightQueue[index - CHUNK_WIDTH] = true;
                }
            }
            else
            {
                // Tell our master engine to queue the next block over
                vector2 block_pos;
                block_pos.x = floor(index % CHUNK_WIDTH) + (pos.x * CHUNK_WIDTH);
                block_pos.y = floor(index / CHUNK_WIDTH) + (pos.y * CHUNK_WIDTH) - 1;
                master->QueueBlock(block_pos);
            }
            
        }
        
        inLightQueue[index] = false;
        LightUpdateQueue.pop();
        budget--;
    }
}

bool Chunk::Update(float dt)
{
    // Random tick (gradual change, growth)
    randomTickTimeTotal += dt;
    while (randomTickTimeTotal > randomTickTimeLimit)
    {
        // std::cout << "Debug: random tick.\n";
        RandomTick();
        randomTickTimeTotal -= randomTickTimeLimit;
    }

    // Block update tick (chain reaction)
    updateTickTimeTotal += dt;
    while (updateTickTimeTotal > updateTickTimeLimit)
    {
        UpdateTick();
        updateTickTimeTotal -= updateTickTimeLimit;
    }

    UpdateLight();
    return false;
}

ChunkEngine::ChunkEngine()
{
    chunk_radius = 4;
    sky_light = 1.0f;
    light_decay = 16;
    world_seed = rd();
    random.seed(world_seed);
    DataItemRegistry::Init();
    BlockRegistry::Init();
    StructureRegistry::Init();
    // Nevermind!
    // world_seed = 1234;
}

int ChunkEngine::GetChunkRadius()
{
    return chunk_radius;
}

void ChunkEngine::SetChunkRadius(int r)
{
    chunk_radius = r;
}

int ChunkEngine::GetSeed()
{
    return world_seed;
}

void ChunkEngine::SetSeed(int v)
{
    world_seed = v;
}

uint32_t ChunkEngine::GetRand()
{
    return random();
}

std::vector<ChunkSave>& ChunkEngine::GetChunkSaves()
{
    return saveData;
}

const std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkHasher>& ChunkEngine::GetChunks()
{
    return chunks;
}

bool ChunkEngine::ChunkExists(ChunkCoord c)
{
    return chunks.find(c) != chunks.end();
}

ChunkSave* ChunkEngine::GetChunkSave(ChunkCoord c)
{
    for (size_t i = 0; i < saveData.size(); i++)
    {
        if (saveData.at(i).coord == c)
        {
            return &saveData.at(i);
        }
    }
    return nullptr;
}

void ChunkEngine::AddChunkSave(ChunkSave save)
{
    saveData.push_back(save);
}

void ChunkEngine::AddBlockToChunkSave(vector2 chunk_pos_block, vector2 pos_in_chunk, BlockID block, BlockData data)
{
    ChunkCoord pos_chunk = {
        static_cast<int>(chunk_pos_block.x / CHUNK_WIDTH),
        static_cast<int>(chunk_pos_block.y / CHUNK_WIDTH)
    };

    ChunkSave* save = GetChunkSave(pos_chunk);
    if (save != nullptr)
    {
        int index = pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH);
        bool block_exists = false;
        int block_list_index = 0;
        
        for (size_t i = 0; i < save->positions.size(); i++)
        {
            if (save->positions[i] == index)
            {
                block_exists = true;
                block_list_index = i;
            }
        }

        if (block_exists)
        {
            save->blocks.at(block_list_index) = block;
            save->data.at(block_list_index) = data;
        }
        else
        {
            save->blocks.push_back(block);
            save->data.push_back(data);
            save->positions.push_back(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
        }
    }
    else
    {
        ChunkSave newChunkSave;
        newChunkSave.coord = pos_chunk;
        newChunkSave.blocks.push_back(block);
        newChunkSave.data.push_back(data);
        newChunkSave.positions.push_back(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
        AddChunkSave(newChunkSave);
    }
}

void ChunkEngine::CreateChunk(const ChunkCoord& c)
{
    auto itPair = chunks.emplace(c, std::make_unique<Chunk>(c, world_seed, this));
    ChunkSave* save = GetChunkSave(c);
    if (save != nullptr)
    {
        itPair.first->second->GenerateSave(save);
    }
}

int ChunkEngine::UpdateChunks(float dt)
{
    // std::cout << "Debug: updating rand.\n";
    int count = 0;
    for (auto i = chunks.begin(); i != chunks.end(); ++i)
    {
        const auto& pair = *i;
        pair.second->Update(dt);
        count++;
    }
    return count;
}

void ChunkEngine::UpdateWorld(vector2 pos)
{
    int cam_chunk_x = static_cast<int>(floor(pos.x / static_cast<float>(CHUNK_WIDTH * BLOCK_WIDTH)));
    int cam_chunk_y = static_cast<int>(floor(pos.y / static_cast<float>(CHUNK_WIDTH * BLOCK_WIDTH)));
    int budget = 10;

    auto it = chunks.find({cam_chunk_x, cam_chunk_y});
    if (it == chunks.end())
    {
        // Chunk does not exist
        CreateChunk({cam_chunk_x, cam_chunk_y});
        budget--;
    }

    // Spawn new chunks / track what chunks are in our desired zone
    for (int dx = -chunk_radius; dx <= chunk_radius; dx++)
    {
        for (int dy = -chunk_radius; dy <= chunk_radius; dy++)
        {
            ChunkCoord target = {
                cam_chunk_x + dx,
                cam_chunk_y + dy
            };
            
            auto it = chunks.find(target);
            if (it == chunks.end() && budget > 0)
            {
                // Chunk does not exist
                CreateChunk(target);
                budget--;
            }
        }
    }

    // Remove old chunks
    for (const auto& chunk : chunks)
    {
        if (abs(chunk.first.x - cam_chunk_x) > chunk_radius + 1 || abs(chunk.first.y - cam_chunk_y) > chunk_radius + 1)
        {
            chunks.erase(chunk.first);
        }
    }
    
}

vector2 ChunkEngine::CollidePoint(vector2 pos)
{
    // Get the chunk that the position is in
    vector2 pos_block = {
        static_cast<int>(floor(pos.x / BLOCK_WIDTH)),
        static_cast<int>(floor(pos.y / BLOCK_WIDTH))
    };

    ChunkCoord pos_chunk = {
        static_cast<int>(floor(pos_block.x / CHUNK_WIDTH)),
        static_cast<int>(floor(pos_block.y / CHUNK_WIDTH))
    };

    auto it = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        // Adjust to get the relative position
        vector2 chunk_blockPos = {
            pos_chunk.x * CHUNK_WIDTH,
            pos_chunk.y * CHUNK_WIDTH
        };
        // Round to block position
        vector2 relative_block_pos = pos_block - chunk_blockPos;

        int block = it->second->GetBlock(relative_block_pos.x, relative_block_pos.y);
        BlockData data = it->second->GetBlockData(relative_block_pos.x, relative_block_pos.y);
        BlockDef def = BlockRegistry::get(block);
        
        bool canOpen = def.Find("doorOpen");

        bool collide = def.isSolid;
        if (canOpen) collide = collide && static_cast<bool>(def.Read("doorOpen", &data)) == false;

        // Return the value of that block
        if (collide)
        {
            // Get the fix upwards and leftwards
            vector2 block_world = {
                (pos_block.x * BLOCK_WIDTH),
                (pos_block.y * BLOCK_WIDTH)
            };

            vector2 penetration = {
                block_world.x - pos.x,
                block_world.y - pos.y
            };

            vector2 fix;

            // Flip to closest edge
            if (abs(penetration.x) > BLOCK_WIDTH / 2) fix.x = penetration.x + BLOCK_WIDTH;
            else fix.x = penetration.x - 0.001f;

            if (abs(penetration.y) > BLOCK_WIDTH / 2) fix.y = penetration.y + BLOCK_WIDTH;
            else fix.y = penetration.y - 0.001f;

            // std::cout << "Debug: fix = (" << fix.x << ", " << fix.y << ")" << std::endl;
            return fix;
        }
        else
        {
            return {0, 0};
        }

    }
    // collsion is outside of existing chunks
    else return {0, 0};
}

BlockID ChunkEngine::GetBlock(vector2 pos_block)
{
    ChunkCoord pos_chunk = {
        static_cast<int>(floor(pos_block.x / CHUNK_WIDTH)), 
        static_cast<int>(floor(pos_block.y / CHUNK_WIDTH))
    };

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_blockPos = {
            pos_chunk.x * CHUNK_WIDTH,
            pos_chunk.y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = pos_block - chunk_blockPos;
        BlockID block = it->second->GetBlock(pos_in_chunk.x, pos_in_chunk.y);
        return block;
    }
    return 0;
}

void ChunkEngine::SetBlock(vector2 pos_block, BlockID block, BlockData data, UpdateType updateType)
{
    ChunkCoord pos_chunk = {
        static_cast<int>(floor(pos_block.x / CHUNK_WIDTH)), 
        static_cast<int>(floor(pos_block.y / CHUNK_WIDTH))
    };

    auto it  = chunks.find(pos_chunk);

    vector2 chunk_blockPos = {
        pos_chunk.x * CHUNK_WIDTH,
        pos_chunk.y * CHUNK_WIDTH
    };
    vector2 pos_in_chunk = pos_block - chunk_blockPos;

    if (it != chunks.end())
    {
        
        it->second->SetBlock(pos_in_chunk.x, pos_in_chunk.y, block);
        it->second->SetBlockData(pos_in_chunk.x, pos_in_chunk.y, data);
        it->second->AddToLightQueue(pos_in_chunk.x, pos_in_chunk.y);

        // Queue the block update and the four adjacent blocks
        switch (updateType)
        {
            case UpdateType::instant:
                it->second->QueueBlockUpdate(pos_in_chunk.x, pos_in_chunk.y);
                break;
            case UpdateType::defered:
                it->second->DeferBlockUpdate(pos_in_chunk.x, pos_in_chunk.y);
                break;
            case UpdateType::softDefered:
                it->second->SoftDeferBlockUpdate(pos_in_chunk.x, pos_in_chunk.y);
                break;
        }
        it->second->SoftDeferBlockUpdate(pos_in_chunk.x + 1, pos_in_chunk.y); // right
        it->second->SoftDeferBlockUpdate(pos_in_chunk.x - 1, pos_in_chunk.y); // left
        it->second->SoftDeferBlockUpdate(pos_in_chunk.x, pos_in_chunk.y + 1); // down
        it->second->SoftDeferBlockUpdate(pos_in_chunk.x, pos_in_chunk.y - 1); // up
        
    }
    AddBlockToChunkSave(chunk_blockPos, pos_in_chunk, block, data);

}

BlockData ChunkEngine::GetBlockData(vector2 pos_block)
{
    ChunkCoord pos_chunk = {
        static_cast<int>(floor(pos_block.x / CHUNK_WIDTH)), 
        static_cast<int>(floor(pos_block.y / CHUNK_WIDTH))
    };

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_blockPos = {
            pos_chunk.x * CHUNK_WIDTH,
            pos_chunk.y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = pos_block - chunk_blockPos;
        BlockData blockData = it->second->GetBlockData(pos_in_chunk.x, pos_in_chunk.y);
        return blockData;
    }
    return 0;
}

BlockData* ChunkEngine::TakeBlockData(vector2 worldPos_block)
{
    ChunkCoord pos_chunk = {
        static_cast<int>(floor(worldPos_block.x / CHUNK_WIDTH)), 
        static_cast<int>(floor(worldPos_block.y / CHUNK_WIDTH))
    };

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_blockPos = {
            pos_chunk.x * CHUNK_WIDTH,
            pos_chunk.y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = worldPos_block - chunk_blockPos;
        BlockData* data = it->second->TakeBlockData(pos_in_chunk);
        return data;
    }
    return nullptr;
}

std::pair<int, int> ChunkEngine::GetLight(vector2 block_pos)
{
    int pos_chunk_x = int(floor(block_pos.x / CHUNK_WIDTH));
    int pos_chunk_y = int(floor(block_pos.y / CHUNK_WIDTH));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_worldPos = {
            pos_chunk_x * CHUNK_WIDTH,
            pos_chunk_y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = block_pos - chunk_worldPos;

        return it->second->GetLight(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
    }
    else
    {
        return {0, 0};
    }
}

std::pair<bool, BlockID> ChunkEngine::MineBlock(vector2 pos_block, float mining)
{
    BlockID block = GetBlock(pos_block);
    const BlockDef def = BlockRegistry::get(block);
    if (mining >= def.mineStrength && def.mineable)
    {
        SetBlock(pos_block, BlockRegistry::getIDByName("air"), 0, UpdateType::instant);
        return {true, block};
    }
    return {false, 0};
}

bool ChunkEngine::BuildBlock(vector2 pos_block, BlockID block)
{
    BlockID block_old = GetBlock(pos_block);
    const BlockDef& def = BlockRegistry::get(block_old);
    if (def.placeOver)
    {
        SetBlock(pos_block, block, 0, UpdateType::instant);
        return true;
    }
    else
    {
        ChunkCoord chunkPos = {
            static_cast<int>(floor(pos_block.x / CHUNK_WIDTH)),
            static_cast<int>(floor(pos_block.y / CHUNK_WIDTH))
        };

        vector2 chunkPos_block = {
            chunkPos.x * CHUNK_WIDTH,
            chunkPos.y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = pos_block - chunkPos_block;
        auto it = chunks.find(chunkPos);
        if (it != chunks.end())
        {
            BlockData* data = TakeBlockData(pos_block);
            BlockDef def = BlockRegistry::get(block_old);
    
            // Create the context for the RandomTick function
            BehaviorContext ctx;
            ctx.block = block_old;
            ctx.data = data;
            ctx.chunk = it->second.get();
            ctx.x = pos_in_chunk.x;
            ctx.y = pos_in_chunk.y;

    
            def.SmartTick(ctx);
        }

    }
    return false;
}

int ChunkEngine::GetSkyLight(vector2 block_pos)
{
    int pos_chunk_x = int(floor(block_pos.x / CHUNK_WIDTH));
    int pos_chunk_y = int(floor(block_pos.y / CHUNK_WIDTH));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_worldPos = {
            pos_chunk_x * CHUNK_WIDTH,
            pos_chunk_y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = block_pos - chunk_worldPos;

        return it->second->GetSkyLight(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
    }
    else
    {
        return 0;
    }
}

int ChunkEngine::GetBlockLight(vector2 block_pos)
{
    int pos_chunk_x = int(floor(block_pos.x / CHUNK_WIDTH));
    int pos_chunk_y = int(floor(block_pos.y / CHUNK_WIDTH));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_worldPos = {
            pos_chunk_x * CHUNK_WIDTH,
            pos_chunk_y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = block_pos - chunk_worldPos;

        return it->second->GetBlockLight(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
    }
    else
    {
        return 0;
    }
}

void ChunkEngine::QueueBlock(vector2 block_pos)
{
    int pos_chunk_x = int(floor(block_pos.x / CHUNK_WIDTH));
    int pos_chunk_y = int(floor(block_pos.y / CHUNK_WIDTH));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_worldPos = {
            float(pos_chunk_x * CHUNK_WIDTH),
            float(pos_chunk_y * CHUNK_WIDTH)
        };

        vector2 pos_in_chunk = block_pos - chunk_worldPos;

        it->second->AddToLightQueue(pos_in_chunk.x, pos_in_chunk.y);
    }
}
