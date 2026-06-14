#pragma once

#include <iostream>
#include <vector>
#include <numbers>
#include <random>
#include <memory>
#include <queue>
#include <algorithm>
#include <functional>
#include <iterator>
#include <FastNoiseLite.h>
#include <string>
#include <list>

#include "figures.hpp"
#include "vector2.hpp"
#include "blockRegistry.hpp"

const int LIGHT_DECAY_SKY = 64;
const int LIGHT_DECAY_BLOCK = 32;

const int foo = 67;

struct ChunkEngine; // Forward declaration

struct ChunkCoord
{
    int x;
    int y;

    bool operator==(const ChunkCoord& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct ChunkHasher
{
    std::size_t operator()(const ChunkCoord& c) const
    {
        // unique hashing algorithm for our coordinate system
        std::size_t h1 = std::hash<int>()(c.x);
        std::size_t h2 = std::hash<int>()(c.y);
        return h1 ^ (h2 << 1);
    }
};

struct ChunkSave
{
    ChunkCoord coord;
    std::vector<int> positions;
    std::vector<int> blocks;
};

struct Chunk
{
    private:
    double deltaTimeTotal;
    double deltaTimeLimit;

    std::vector<BlockID> blocks;
    ChunkCoord pos;
    
    std::vector<int> sky_lights;
    std::vector<int> block_lights;
    
    // Light Propagation Queue
    std::queue<int> LightUpdateQueue;
    std::vector<bool> inLightQueue;

    // Block Update Queue
    std::list<int> BlockUpdateQueue;
    std::list<int> BlockUpdateIgnore;
    
    ChunkEngine* master;
    
    int biome;
    
    public:
    Chunk(ChunkCoord c, int seed, ChunkEngine* _master);

    int GetBiome();

    ChunkCoord GetPosition();

    void SetPosition(ChunkCoord c);

    int GetLightQueLength();

    std::pair<int, int> GetLight(int i);

    std::pair<int, int> TryGetLight(int i, vector2 offset);

    int GetSkyLight(int i);

    int TryGetSkyLight(int index, vector2 offset);

    int GetBlockLight(int i);

    int TryGetBlockLight(int index, vector2 offset);

    void AddToLightQueue(int x, int y);
    void AddToBlockQueue(int x, int y);
    void AddToBlockIgnore(int x, int y);

    int GetBlock(int i);
    int GetBlock(int x, int y);
    void SetBlock(int x, int y, BlockID block);

    int  SafeGetBlock(vector2 pos_block);
    void SafeSetBlock(vector2 pos_block, BlockID block);

    void Generate(ChunkCoord c, int seed);

    void GenerateSave(ChunkSave* save);

    void UpdateBlocks();

    void UpdateLight();
    
    bool Update(float dt);
};

struct ChunkEngine
{
    private:
    std::unordered_map<
        ChunkCoord, 
        std::unique_ptr<Chunk>,
        ChunkHasher
    > chunks;

    std::vector<ChunkSave> saveData;

    // std::vector<ChunkCoord> adresses;

    int world_seed;

    int chunk_radius;
    float sky_light;
    int light_decay;

    public:
    ChunkEngine();

    int  GetChunkRadius();
    void SetChunkRadius(int r);

    int  GetSeed();
    void SetSeed(int v);

    std::vector<ChunkSave>& GetChunkSaves();

    const std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkHasher>& GetChunks();

    bool ChunkExists(ChunkCoord c);

    ChunkSave* ChunkSaveExists(ChunkCoord c);

    auto CreateChunk(const ChunkCoord& c);

    void AddChunkSave(ChunkSave save);

    void AddBlockToChunkSave(vector2 chunk_pos_block, vector2 pos_in_chunk, BlockID block);

    int UpdateChunks(float dt);

    void UpdateWorld(vector2 pos);

    vector2 CollidePoint(vector2 pos);

    // vector2 GetCollision(vector2 pos);

    BlockID GetBlock(vector2 pos_block);

    void SetBlock(vector2 pos_block, BlockID block);
    
    std::pair<bool, BlockID> MineBlock(vector2 pos, float mining);
    
    bool BuildBlock(vector2 pos_block, BlockID block);

    std::pair<int, int> GetLight(vector2 block_pos);

    int GetSkyLight(vector2 block_pos);

    int GetBlockLight(vector2 block_pos);

    void QueueBlock(vector2 block_pos);

};