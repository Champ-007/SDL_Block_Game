
#include "chunks.hpp"

Chunk::Chunk(ChunkCoord c, int seed, ChunkEngine* _master)
{
    blocks.resize(CHUNK_LENGTH);
    sky_lights.resize(CHUNK_LENGTH);
    block_lights.resize(CHUNK_LENGTH);
    in_queue.resize(CHUNK_LENGTH);
    edge_light.resize(CHUNK_WIDTH * 4);
    pos = c;
    master = _master;
    Generate(c, seed);
    // UpdateLight();
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
    return LUQ.size();
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

void Chunk::AddBlockToQueue(int x, int y)
{
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_WIDTH)
    {
        if (!in_queue[x + (y * CHUNK_WIDTH)])
        {
            // std::cout << "Debug: adding queue at " << x + (y * CHUNK_WIDTH) << std::endl;
            LUQ.push(x + (y * CHUNK_WIDTH));
            in_queue[x + (y * CHUNK_WIDTH)] = true;
        }
    }
    else
    {
        // std::cout << "Error: bad queue attempt." << std::endl;
    }
}

int Chunk::GetBlock(int x, int y)
{
    return blocks[x + (y * CHUNK_WIDTH)];
}

int Chunk::GetBlock(int i)
{
    return blocks[i];
}

int Chunk::SetBlock(int x, int y)
{
    int new_block = 0;
    switch (blocks[x + (y * CHUNK_WIDTH)])
    {
        case -1:
            new_block = 136;
            break;
    
        case 136:
            new_block = 205;
            break;

        case 205:
            new_block = -1;
            break;

        default:
            new_block = -1;
            break;
    }

    blocks[x + (y * CHUNK_WIDTH)] = new_block;
    return new_block;
}

void Chunk::Generate(ChunkCoord c, int seed)
{
    // noise object for general terrain density
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.06f);
    noise.SetSeed(seed);
    
    // Noise object for basic terrain details
    FastNoiseLite noise2;
    noise2.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise2.SetFrequency(0.35f);
    noise2.SetSeed(seed + 1);

    for (int y = 0; y < CHUNK_WIDTH; y++)
    {
        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            int index_x = x + (c.x * CHUNK_WIDTH);
            int index_y = y + (c.y * CHUNK_WIDTH);
            float density  = noise.GetNoise((float)index_x, (float)index_y);
            float density2 = noise2.GetNoise((float)index_x, (float)index_y);
            int i = x + (y * CHUNK_WIDTH);
            sky_lights[i] = 0;
            block_lights[i] = 0;
            if (density < 0.0f)
            {
                if (density2 < -0.3 && noise.GetNoise((float)index_x, (float)index_y + 1) > 0.0f)
                {
                    blocks[i] = 205;
                }
                else
                {
                    blocks[i] = -1;
                }
            }
            else if (density < 0.4)
            {
                if (noise.GetNoise((float)index_x, (float)index_y - 1) < 0.0f)
                {
                    blocks[i] = 320;
                }
                else
                {
                    blocks[i] = 136;
                }
            }
            else
            {
                if (density2 < -0.3f)
                {
                    blocks[i] = 303;
                }
                else
                {
                    blocks[i] = 339;
                }
            }
            in_queue[i] = true;
            LUQ.push(i);
        }
    }

    for (int i = 0; i < CHUNK_WIDTH * 4; i++)
    {
        edge_light[i] = 0;
    }
    
}

void Chunk::GenerateSave(ChunkSave* save)
{
    for (size_t i = 0; i < save->blocks.size(); i++)
    {
        // std::cout << "Debug: setting block to " << save->blocks.at(i) << std::endl; 
        blocks.at(save->positions.at(i)) = save->blocks.at(i);
        if (save->blocks.at(i) == -1)
        {
            LUQ.push(save->positions.at(i));
            in_queue[save->positions.at(i)] = true;
        }
    }
    
}

void Chunk::UpdateLight()
{
    int budget = 500;
    while (!LUQ.empty() && budget > 0) 
    {
        int block_highest = 0;
        int sky_highest = 0;
        int index = LUQ.front();
        vector2 pos = {
            index % CHUNK_WIDTH,
            floor(index / CHUNK_WIDTH)
        };
        int block_type = blocks[index];
        int sky_light_tran = sky_lights[index];
        int block_light_tran = block_lights[index];
        
        // Get the highest surrounding light for block
        int sky_light;
        int block_light;

        // To the right
        sky_light = TryGetSkyLight(index, {1, 0});
        if (sky_light > sky_highest)
        {
            sky_highest = sky_light;
        }
        block_light = TryGetBlockLight(index, {1, 0});
        if (block_light > block_highest)
        {
            block_highest = block_light;
        }

        // To the left
        sky_light = TryGetSkyLight(index, {-1, 0});
        if (sky_light > sky_highest)
        {
            sky_highest = sky_light;
        }
        block_light = TryGetBlockLight(index, {-1, 0});
        if (block_light > block_highest)
        {
            block_highest = block_light;
        }

        // To the up
        sky_light = TryGetSkyLight(index, {0, -1});
        if (sky_light > sky_highest)
        {
            sky_highest = sky_light;
        }
        block_light = TryGetBlockLight(index, {0, -1});
        if (block_light > block_highest)
        {
            block_highest = block_light;
        }

        // To the down
        sky_light = TryGetSkyLight(index, {0, 1});
        if (sky_light > sky_highest)
        {
            sky_highest = sky_light;
        }
        block_light = TryGetBlockLight(index, {0, 1});
        if (block_light > block_highest)
        {
            block_highest = block_light;
        }

        // raise light of block to either -1 of highest adjacent light, or native light of block, whichever is higher
        int sky_native = 0;
        int block_native = 0;
        if (block_type == -1)
        {
            sky_native = 255;
        }
        if (block_type == 205)
        {
            sky_native = 255;
            block_native = 255;
        }
        sky_lights[index] = std::max({sky_native, sky_highest - LIGHT_DECAY_SKY, 0});
        block_lights[index] = std::max({block_native, block_highest - LIGHT_DECAY_BLOCK, 0});
        
        // Propogate the update
        if (sky_lights[index] != sky_light_tran || block_lights[index] != block_light_tran)
        {
            // To the right
            if (index % CHUNK_WIDTH != CHUNK_WIDTH - 1)
            {
                if (!in_queue[index + 1])
                {
                    LUQ.push(index + 1);
                    in_queue[index + 1] = true;
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
                if (!in_queue[index - 1])
                {
                    LUQ.push(index - 1);
                    in_queue[index - 1] = true;
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
                if (!in_queue[index + CHUNK_WIDTH])
                {
                    LUQ.push(index + CHUNK_WIDTH);
                    in_queue[index + CHUNK_WIDTH] = true;
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
                if (!in_queue[index - CHUNK_WIDTH])
                {
                    LUQ.push(index - CHUNK_WIDTH);
                    in_queue[index - CHUNK_WIDTH] = true;
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
        
        in_queue[index] = false;
        LUQ.pop();
        budget--;
    }
}

bool Chunk::Update()
{
    UpdateLight();
    return false;
}

ChunkEngine::ChunkEngine()
{
    std::random_device rd;
    world_seed = rd();
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

ChunkSave* ChunkEngine::ChunkSaveExists(ChunkCoord c)
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

auto ChunkEngine::CreateChunk(const ChunkCoord& c)
{
    ChunkSave* save = ChunkSaveExists(c);
    if (save != nullptr)
    {
        auto chunkPair = chunks.emplace(c, std::make_unique<Chunk>(c, world_seed, this));
        if (chunkPair.second)
        {
            chunkPair.first->second.get()->GenerateSave(save);
        }
        return chunkPair;
    }
    else
    {
        return chunks.emplace(c, std::make_unique<Chunk>(c, world_seed, this));
    }
}

int ChunkEngine::UpdateChunks()
{
    int count = 0;
    for (auto i = chunks.begin(); i != chunks.end(); ++i)
    {
        const auto& pair = *i;
        pair.second->Update();
        count++;
    }
    return count;
}

void ChunkEngine::UpdateWorld(vector2 pos)
{
    int cam_chunk_x = static_cast<int>(floor(pos.x / static_cast<float>(CHUNK_WIDTH * BLOCK_WIDTH)));
    int cam_chunk_y = static_cast<int>(floor(pos.y / static_cast<float>(CHUNK_WIDTH * BLOCK_WIDTH)));

    // Spawn new chunks / track what chunks are in our desired zone
    // std::vector<ChunkCoord> coords;
    for (int dx = -chunk_radius; dx <= chunk_radius; dx++)
    {
        for (int dy = -chunk_radius; dy <= chunk_radius; dy++)
        {
            ChunkCoord target = {
                cam_chunk_x + dx,
                cam_chunk_y + dy
            };
            
            auto [it, inserted] = CreateChunk(target);

            // coords.push_back(target);
        }
    }

    // Remove old chunks
    for (const auto& chunk : chunks)
    {
        bool is_in = false;
        if (abs(chunk.first.x - cam_chunk_x) > chunk_radius + 1 || abs(chunk.first.y - cam_chunk_y) > chunk_radius + 1)
        {
            chunks.erase(chunk.first);
        }
    }
    
}

void ChunkEngine::MineBlock(vector2 pos_block)
{
    int pos_chunk_x = static_cast<int>(floor(pos_block.x / CHUNK_WIDTH));
    int pos_chunk_y = static_cast<int>(floor(pos_block.y / CHUNK_WIDTH));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    auto it  = chunks.find(pos_chunk);

    if (it != chunks.end())
    {
        vector2 chunk_blockPos = {
            pos_chunk_x * CHUNK_WIDTH,
            pos_chunk_y * CHUNK_WIDTH
        };

        vector2 pos_in_chunk = pos_block - chunk_blockPos;

        int block = it->second->SetBlock(pos_in_chunk.x, pos_in_chunk.y);
        it->second->AddBlockToQueue((int)pos_in_chunk.x, (int)pos_in_chunk.y);
        ChunkSave* save = ChunkSaveExists({pos_chunk_x, pos_chunk_y});
        if (save != nullptr)
        {
            int index = pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH);
            bool block_exists = false;
            int block_list_index = 0;
            for (int i = 0; i < save->positions.size(); i++)
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
            }
            else
            {
                save->blocks.push_back(block);
                save->positions.push_back(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
            }
        }
        else
        {
            ChunkSave newChunkSave;
            newChunkSave.coord = {pos_chunk_x, pos_chunk_y};
            newChunkSave.blocks.push_back(block);
            newChunkSave.positions.push_back(pos_in_chunk.x + (pos_in_chunk.y * CHUNK_WIDTH));
            AddChunkSave(newChunkSave);
        }
    }
}

vector2 ChunkEngine::GetCollision(vector2 pos)
{
    // Initialize our response
    vector2 collision = {0, 0};

    // // Check tops points for worst collision
    // for (auto& point : col.top_collision_points)
    // {
    //     vector2 point_world = point + pos;
    //     PointCollisionInfo collision_result = CollidePoint(point_world);
    //     if (collision_result.block_type != 0 && collision_result.depth > collision.top)
    //     {
    //         collision.top = collision_result.depth;
    //     }
    // }
    // // Check right points for worst collision
    // for (auto& point : col.right_collision_points)
    // {
    //     vector2 point_world = point + pos;
    //     PointCollisionInfo collision_result = CollidePoint(point_world, Collision_right);
    //     if (collision_result.block_type != 0 && collision_result.depth < collision.right)
    //     {
    //         collision.right = collision_result.depth;
    //     }
    // }
    // // Check bottom points for worst collision
    // for (auto& point : col.bottom_collision_points)
    // {
    //     vector2 point_world = point + pos;
    //     PointCollisionInfo collision_result = CollidePoint(point_world, Collision_bottom);
    //     if (collision_result.block_type != 0 && collision_result.depth < collision.bottom)
    //     {
    //         collision.bottom = collision_result.depth;
    //     }
    // }
    // // Check left points for worst collision
    // for (auto& point : col.left_collision_points)
    // {
    //     vector2 point_world = point + pos;
    //     PointCollisionInfo collision_result = CollidePoint(point_world, Collision_left);
    //     if (collision_result.block_type != 0 && collision_result.depth > collision.left)
    //     {
    //         collision.left = collision_result.depth;
    //     }
    // }

    // Get the block at our position
    
    // The width of a chunk in world space
    int chunk_world_width = CHUNK_WIDTH * BLOCK_WIDTH;

    // Round our position to the nearest chunk position, in chunk space
    int pos_chunk_x = int(floor(pos.x / chunk_world_width));
    int pos_chunk_y = int(floor(pos.y / chunk_world_width));
    ChunkCoord pos_chunk = {pos_chunk_x, pos_chunk_y};

    // Look up that chunk to see if it exists
    auto it = chunks.find(pos_chunk);
    if (it != chunks.end())
    {
        // round our position to block space
        vector2 pos_blocks = {
            int(floor(pos.x / BLOCK_WIDTH)),
            int(floor(pos.y / BLOCK_WIDTH))
        };

        // Get the block space position of the selected chunk
        vector2 chunk_worldPos = {
            pos_chunk_x * CHUNK_WIDTH,
            pos_chunk_y * CHUNK_WIDTH
        };

        // Get the block position of our position relative to the selected chunk
        vector2 pos_block_in_chunk = pos_blocks - chunk_worldPos;

        // Get what block our position is over top of
        int block = it->second->GetBlock(int(pos_block_in_chunk.x), int(pos_block_in_chunk.y));

        // If we are colliding with something, get the most efficient lateral and vertical moves to exit that block
        if (block == -1 || block == 205)
        {
            // Ignore collision for air and flowers
        }
        else
        {
            // Get the world position of the colliding block
            vector2 block_world = pos_blocks * BLOCK_WIDTH;

            // Get vector2 relative to the origin of the colliding block
            vector2 diff = {
                pos.x - block_world.x,
                pos.y - block_world.y
            };

            // If we are on the close side of the block laterally,
            // then return the inverse, but if we are on the far
            // side, then project the change to the opposite side
            if (diff.x < BLOCK_WIDTH / 2)
            {
                collision.x = -diff.x;
                // std::cout << "Debug: collision says go left: " << collision.x << std::endl;
            }
            else
            {
                collision.x = -diff.x + BLOCK_WIDTH;
                // std::cout << "Debug: Collision says go right: " << collision.x << 
            }

            // same calculation for vertical direction, get the value
            // that represents the smallest vertical movement to leave the block
            if (diff.y < BLOCK_WIDTH / 2)
            {
                collision.y = -diff.y;
            }
            else
            {
                collision.y = -diff.y + BLOCK_WIDTH;
            }
        }
    }

    return collision;
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

        it->second->AddBlockToQueue(pos_in_chunk.x, pos_in_chunk.y);
    }
}

