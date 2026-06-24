
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <string>

#include "player.hpp"
#include "chunks.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "vector2.hpp"

const int WIDTH = 800, HEIGHT = 600;
int DISPLAY_WIDTH = 0, DISPLAY_HEIGHT = 0;
int VARIABLE = 0;

float RandRange(const float rand, const float min, const float max)
{
    if (max < min)
    {
        std::cout << "Warning: nonsense random range." << std::endl;
        return 0;
    }
    else
    {
        float output = rand;
        float diff = max - min;
        output = output * diff;
        output = output + min;
        return output;
    }
}

void DestroySDL(SDL_Window* window, SDL_Renderer* renderer)
{
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
    }
    IMG_Quit();
    SDL_Quit();
}

struct generateTexturesInfo
{
    bool good;
    std::vector<SDL_Texture*> textures;
};

generateTexturesInfo GenerateTextures(SDL_Renderer* renderer, const int assetsc, const char* assetsv[])
{
    std::vector<SDL_Texture*> textures;
    bool genGood = true;
    // std::cout << "Debug: Generating " << assetsc << " textures." << std::endl;
    for (int i = 0; i < assetsc && genGood; ++i)
    {
        // std::cout << "Debug: Attempting to load image: " << assetsv[i] << std::endl;
        SDL_Surface* surface = IMG_Load(assetsv[i]);
        //std::cout << "Debug: IMG_Load returned, error: " << IMG_GetError() << std::endl;
        //std::cout << "Debug: surface->w before null check: " << (surface ? surface->w : -1) << std::endl;
        if (surface == nullptr)
        {
            std::cerr << "SDL failed to create texture: " << assetsv[i] << ", IMG error: " << IMG_GetError() << std::endl;
            genGood = false;
        }
        else
        {
            // Create texture and add it to list
            //std::cout << "Debug: renderer = " << renderer << " surface = " << surface << std::endl;
            // Before SDL_CreateTextureFromSurface:
            //std::cout << "Debug: Surface format: " << surface->format->format << std::endl;
            //std::cout << "Debug: Surface size: " << surface->w << " x " << surface->h << std::endl;
            //std::cout << "Debug: Surface pitch: " << surface->pitch << std::endl;
            
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            // std::cout << "Debug: successfully created texture: " << assetsv[i] << " Attempted SDL get error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            //std::cout << "Debug: successfully freed surface." << std::endl;
            if (texture == nullptr)
            {
                std::cerr << "SDL failed to convert surface to texture. SDL error: " << SDL_GetError() << std::endl;
                genGood = false;
            }
            else
            {
                textures.push_back(texture);
            }
            // std::cout << "Debug: process completed for: " << assetsv[i] << std::endl;
        }
    }

    if (genGood)
    {
        // std::cout << "Debug: generation was good. Returning vector of " << textures.size() << " textures." << std::endl;
        return {true, textures};
    }
    else
    {
        // Generation was bad, destroy all textures before and tell the main to quit the program.
        for (int j = 0; j < (int)textures.size(); ++j)
        {
            SDL_DestroyTexture(textures[j]);
        }
        return {false, textures};
    }
}

void DestroyTextures(std::vector<SDL_Texture*>& textures)
{
    for (SDL_Texture* tex : textures)
    {
        if (tex != nullptr)
        {
            SDL_DestroyTexture(tex);
        }
    }
    textures.clear();

}

int LoadSave(std::string str, ChunkEngine& engine, Player& player)
{
    // Open file
    std::ifstream file(str);
    std::string input;

    // Get for opening issues
    if (!file.is_open())
    {
        std::cerr << "Debug: No save found." << std::endl;
        return 1;
    }
    
    // Read each line
    while (file >> input)
    {
        // std::cout << input << std::endl;
        if (input == "seed")
        {
            int item;
            file >> item;
            engine.SetSeed(item);
        }

        if (input == "position")
        {
            int itemx;
            int itemy;
            file >> itemx;
            file >> itemy;
            player.SetPosition({itemx, itemy});
        }

        if (input == "inventory")
        {
            BlockID id;
            file >> id;
            int count;
            file >> count;

            player.PickUpItem(id, count);

            std::string item;
            file >> item;
            while (item != "endInventory")
            {
                BlockID id = std::atoi(item.c_str());
                file >> item;
                int count = std::atoi(item.c_str());
                player.PickUpItem(id, count);
                file >> item;
            }
        }

        if (input == "chunk")
        {
            int x;
            file >> x;
            int y;
            file >> y;
            // std::cout << "position = (" << x << ", " << y << ")" << std::endl;
            ChunkSave saveItem;
            saveItem.coord = {x, y};
            std::string item;
            file >> item;
            while (item != "endchunk")
            {
                saveItem.positions.push_back(std::atoi(item.c_str()));
                file >> item;
                saveItem.blocks.push_back(std::atoi(item.c_str()));
                file >> item;
                saveItem.data.push_back(std::atoi(item.c_str()));
                file >> item;
            }
            engine.AddChunkSave(saveItem);
        }
    }

    file.close();
    return 0;
}

int BakeSave(std::string str, ChunkEngine& engine, Player& player)
{
    std::ofstream writeFile(str);
    writeFile.clear();
    writeFile << "seed " << engine.GetSeed() << ' ';
    // std::cout << "position " << round(player.GetPosition().x) << " " << round(player.GetPosition().y) << '\n';
    writeFile << "position " << round(player.GetPosition().x) << " " << round(player.GetPosition().y) << ' ';
    if (!player.GetInventory().empty())
    {
        writeFile << "inventory ";
        for (auto& item : player.GetInventory())
        {
            writeFile << item.first << ' ' << item.second << " ";
        }
        writeFile << "endInventory ";
    }
    for (auto i : engine.GetChunkSaves())
    {
        writeFile << "chunk " << i.coord.x << " " << i.coord.y << ' ';

        for (size_t j = 0; j < i.blocks.size(); j++)
        {
            writeFile << i.positions.at(j) << " " << i.blocks.at(j) << ' ' << static_cast<int>(i.data.at(j)) << " ";
        }
        writeFile << "endchunk ";
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // Command line variable
    // if (argc == 1)
    // {
    //     VARIABLE = std::stoi(argv[0]);
    // }
    // // LIGHT_DECAY = VARIABLE;
    // std::cout << "Debug: VARIABLE = " << VARIABLE << std::endl;

    // Variable declaration
    SDL_Window* SDLwindow = nullptr;
    SDL_Renderer* SDLrenderer = nullptr;
    SDL_Event SDLwindowEvent;
    int imageAssetsc = 5;
    const char* imageAssetsv[] = {
        "assets/environment/day.jpg", 
        "assets/environment/night3.jpg",
        "assets/characters/man_idle1.png",
        "assets/minecraft/atlas_v2.png",
        "assets/symbols/textAtlas.jpg"
    };
    // const char* imageAssetsn[] = {
    //     "day_background", 
    //     "night_background",
    //     "man_1",
    //     "atlas",
    //     "text"
    // };

    float dt          = 0.0f;
    int SDL_ticks     = SDL_GetTicks64();
    int SDL_ticks_old = SDL_GetTicks64();
    
    ChunkEngine chunk_engine;

    Player player({0, 0}, 12, 28);

    double day_speed = 0.00003f;
    float day_time = 0.0f;
    float day_light = 0.0f;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL to initialize. SDL error: " << SDL_GetError() << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        std::cerr << "SDL_image failed to initialize. SDL error: " << IMG_GetError() << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }

    // Initialize Window
    SDLwindow = SDL_CreateWindow("SDL Project 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (SDLwindow == nullptr)
    {
        std::cerr << "SDL failed to init SDLwindow. SDL error: " << SDL_GetError() << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }

    // Get display measurements
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        std::cerr << "SDL failed to get current display mode. SDL Error: " << SDL_GetError() << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }
    
    // Initialize renderer
    SDLrenderer = SDL_CreateRenderer(SDLwindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
    if (SDLrenderer == nullptr)
    {
        std::cerr << "SDL failed to create SDLrenderer. SDL error: " << SDL_GetError() << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }
    
    // Debug after SDL_CreateRenderer
    SDL_RendererInfo rendererInfo;
    if (SDL_GetRendererInfo(SDLrenderer, &rendererInfo) == 0) {
        std::cout << "Debug: Renderer name: " << rendererInfo.name << std::endl;
        std::cout << "Debug: Renderer flags: " << rendererInfo.flags << std::endl;
    }

    // Initialize assets into textures
    std::vector<SDL_Texture*> textures;
    generateTexturesInfo texturesi = GenerateTextures(SDLrenderer, imageAssetsc, imageAssetsv);
    if (texturesi.good == false)
    {
        std::cerr << "Texture generation failed." << std::endl;
        DestroySDL(SDLwindow, SDLrenderer);
        return 1;
    }
    else
    {
        textures = texturesi.textures;
    }
    
    // std::cout << "Debug: main recieved textures: " << textures.size() << std::endl;
    
    // Create Game Renderer
    GameRenderer gameRenderer(SDLrenderer);
    gameRenderer.SetSkyLight(0.6f);

    // Maximize window
    DISPLAY_WIDTH = displayMode.w;
    DISPLAY_HEIGHT = displayMode.h;
    SDL_SetWindowFullscreen(SDLwindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

    Camera* camera = gameRenderer.GetCamera();
    camera->setDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Read from file
    LoadSave("save1.txt", chunk_engine, player);
    // std::cout << "Debug: player position = " << player.GetPosition().x << " " << player.GetPosition().y << std::endl;
    camera->position = player.GetPosition();
    
    // Prime delta time a little and measure startup time
    SDL_ticks = SDL_GetTicks64();
    dt = (SDL_ticks - SDL_ticks_old);
    std::cout << "Debug: startup delta measures " << dt << std::endl;

    bool skipEarlyPhysics = true;

    bool GAMERUNNING = true;
    while (GAMERUNNING)
    {
        // Process delta time
        SDL_ticks = SDL_GetTicks64();
        dt = (SDL_ticks - SDL_ticks_old);
        SDL_ticks_old = SDL_ticks;
        
        // Handle keyboard input
        const uint8_t* keystates = SDL_GetKeyboardState(NULL);
        
        // I'm pretty sure this is for the x button in the corner of the window. Kill switch
        while (SDL_PollEvent(&SDLwindowEvent))
        {
            if (SDLwindowEvent.type == SDL_QUIT)
            {
                GAMERUNNING = false;
            }
        }

        // Another Basic kill switch
        if (keystates[SDL_SCANCODE_F9]) GAMERUNNING = false;
        
        // Update chunk world
        chunk_engine.UpdateWorld(player.GetPosition());
        chunk_engine.UpdateChunks(dt);
        
        // Update Player
        if (skipEarlyPhysics)
        {
            skipEarlyPhysics = false;
        }
        else
        {
            Physics::UpdatePlayer(player, chunk_engine, dt, keystates);
        }

        // Update daytime
        day_time += day_speed * dt;
        day_light = (sin(day_time) + 1) / 2;
        gameRenderer.SetSkyLight(day_light);
        // std::cout << day_light << std::endl;
        
        gameRenderer.UpdateCamera(player, dt);

        // std::cout << "Debug: camera position = (" << camera.position.x << ", " << camera.position.y << ")" << std::endl;
        // if (keystates[SDL_SCANCODE_UP])  {camera.zoomMult += 0.01f * dt;}
        // if (keystates[SDL_SCANCODE_DOWN]){camera.zoomMult -= 0.01f * dt;}
        
        // Call rendering
        gameRenderer.RenderEverything(&textures, chunk_engine, player, dt);

    }

    // Write to file
    BakeSave("save1.txt", chunk_engine, player);

    int value = chunk_engine.GetChunks().size();
    std::cout << "Debug: final chunk count = " << value << std::endl;

    DestroyTextures(textures);
    DestroySDL(SDLwindow, SDLrenderer);

    std::cout << "Debug: Program has closed." << std::endl;
    return EXIT_SUCCESS;
}