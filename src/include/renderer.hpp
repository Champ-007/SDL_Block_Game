#pragma once

// So, what does the renderer do?
// 1. know about textures and rendering.
// 2. hold all of the helper functions used for rendering
// 3. know about chunk data to render

// What dependencies do we need for the renderer?
// 1. SDL -> rendering
// 2. chunks -> rendering the world
// 3. std io -> debugging

#include <SDL.h>
#include <iostream>
#include <vector>
#include <chunks.hpp>
#include <player.hpp>
#include <string>

#include "vector2.hpp"
#include "blockRegistry.hpp"

struct Camera
{
    public:
    vector2 position;
    float   zoomMult;

    int displayWidth;
    int displayHeight;

    Camera() = default;
    Camera(vector2 p, float zoom);

    void setDisplay(int w, int h);
};

struct GameRenderer
{
    private:
    SDL_Renderer* renderer;
    Camera camera;

    float sky_light = 0.6f;

    int FPSRefreshCountdown = 0;
    int FPS;

    public:
    GameRenderer(SDL_Renderer* _renderer);

    float GetSkyLight();
    void  SetSkyLight(float v);

    Camera* GetCamera();
    void UpdateCamera(Player& player, float dt);

    SDL_Point GetTextureSize(SDL_Texture* texture);
    
    float TextureWtoH(SDL_Texture* texture, float w);
    
    float TextureHtoW(SDL_Texture* texture, float h);

    SDL_Color MultBrightness(SDL_Color color, float v);
    
    SDL_Rect SDL_RectRound(SDL_FRect rect);
    
    void SDL_FRectCenter(SDL_FRect& rect);
    vector2 WorldToScreen(vector2 position);
    
    vector2 ScreenToWorld(vector2 position);
    
    void RenderPlayer(SDL_Texture* texture, Player& player, ChunkEngine* engine);
    
    void RenderChunk(std::vector<SDL_Texture*>* textures, Chunk* chunk);
    
    void RenderEverything(std::vector<SDL_Texture*>* textures, ChunkEngine& engine, Player& player, float dt);

    void RenderDebug(std::vector<SDL_Texture*>* textures, float dt);

    void RenderInventory(std::vector<SDL_Texture*>* textures, Player& player);

};
