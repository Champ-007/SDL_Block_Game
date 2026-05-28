#ifndef CHUNKS
#define CHUNKS

#include <iostream>
#include <vector>
#include <numbers>
#include <random>
#include "vector2.hpp"
#include <memory>
#include <queue>
#include <algorithm>
#include <functional>
#include <iterator>
#include <FastNoiseLite.h>
// #include "collider.hpp"
#include "figures.hpp"

const int LIGHT_DECAY_SKY = 64;
const int LIGHT_DECAY_BLOCK = 48;

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
    std::vector<int> blocks;
    ChunkCoord pos;

    std::vector<int> sky_lights;
    std::vector<int> block_lights;
    std::vector<int> edge_light;

    // Light Propagation Queue
    std::queue<int> LUQ;
    std::vector<bool> in_queue;

    ChunkEngine* master;

    public:
    Chunk(ChunkCoord c, int seed, ChunkEngine* _master);

    ChunkCoord GetPosition();

    void SetPosition(ChunkCoord c);

    int GetLightQueLength();

    std::pair<int, int> GetLight(int i);

    std::pair<int, int> TryGetLight(int i, vector2 offset);

    int GetSkyLight(int i);

    int TryGetSkyLight(int index, vector2 offset);

    int GetBlockLight(int i);

    int TryGetBlockLight(int index, vector2 offset);

    void AddBlockToQueue(int x, int y);

    int GetBlock(int i);
    int GetBlock(int x, int y);
    int SetBlock(int x, int y);

    void UpdateEdgeQueue();

    void Generate(ChunkCoord c, int seed);

    void GenerateSave(ChunkSave* save);

    void UpdateLight();
    
    bool Update();
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

    int chunk_radius = 2;
    float sky_light = 1.0f;
    int light_decay = 16;

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

    int UpdateChunks();

    void UpdateWorld(vector2 pos);

    vector2 CollidePoint(vector2 pos);

    vector2 GetCollision(vector2 pos);
    
    void MineBlock(vector2 pos);

    std::pair<int, int> GetLight(vector2 block_pos);

    int GetSkyLight(vector2 block_pos);

    int GetBlockLight(vector2 block_pos);

    void QueueBlock(vector2 block_pos);

};

#endif