
#include <renderer.hpp>

GameRenderer::GameRenderer(SDL_Renderer* _renderer)
{
    renderer = _renderer;
}

SDL_Point GameRenderer::GetTextureSize(SDL_Texture* texture)
{
    SDL_Point size = {0, 0};
    if (texture == nullptr)
    {
        std::cerr << "Failed to get texture size. texture pointer is null." << std::endl;
    }
    else
    {
        SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
    }
    return size;
}

float GameRenderer::TextureWtoH(SDL_Texture* texture, float w)
{
    SDL_Point size = GetTextureSize(texture);
    return w * ((float)size.y / size.x);
}

float GameRenderer::TextureHtoW(SDL_Texture* texture, float h)
{
    SDL_Point size = GetTextureSize(texture);
    return h * ((float)size.x / size.y);
}

SDL_Rect GameRenderer::SDL_RectRound(SDL_FRect rect)
{
    SDL_Rect newRect = {(int)std::round(rect.x), (int)std::round(rect.y), (int)std::round(rect.w), (int)std::round(rect.h)};
    return newRect;
}

void GameRenderer::SDL_FRectCenter(SDL_FRect& rect)
{
    rect = {rect.x - (rect.w / 2), rect.y - (rect.h / 2), rect.w, rect.h};
}

Camera::Camera(vector2 p, float zoom)
{
    position = p;
    zoomMult = zoom;
}

void Camera::setDisplay(int w, int h)
{
    displayWidth  = w;
    displayHeight = h;
}

float GameRenderer::GetSkyLight()
{
    return sky_light;
}

void GameRenderer::SetSkyLight(float v)
{
    sky_light = v;
}

vector2 GameRenderer::WorldToScreen(vector2 position, Camera camera)
{
    return (position - camera.position) * camera.zoomMult;
}

vector2 GameRenderer::ScreenToWorld(vector2 position, Camera camera)
{
    return {
        (position.x - (camera.displayWidth  / 2)) / camera.zoomMult + (camera.displayWidth  / 2) + camera.position.x,
        (position.y - (camera.displayHeight / 2)) / camera.zoomMult + (camera.displayHeight / 2) + camera.position.y
    };
}

/*
void GameRenderer::RenderPlayer(SDL_Texture* texture, Player& player, SDL_Point size, Camera camera)
{
    vector2 position = player.GetPosition();
    vector2 screenPosition = WorldToScreen(position, camera);
    SDL_FRect frect = {screenPosition.x, screenPosition.y, static_cast<float>(size.x) * camera.zoomMult, static_cast<float>(size.y) * camera.zoomMult};
    SDL_FRectCenter(frect);
    SDL_RenderCopyF(renderer, texture, NULL, &frect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // vector2 depthPosition = WorldToScreen(vector2(player.GetDepth().x, 0) + player.GetPosition(), camera);
    // SDL_RenderDrawLine(renderer, screenPosition.x, screenPosition.y, depthPosition.x, depthPosition.y);
    // depthPosition = WorldToScreen(vector2(0, player.GetDepth().y) + player.GetPosition(), camera);
    // SDL_RenderDrawLine(renderer, screenPosition.x, screenPosition.y, depthPosition.x, depthPosition.y);

    int x_pos = player.GetCursorPosition().x * BLOCK_WIDTH;
    int y_pos = player.GetCursorPosition().y * BLOCK_WIDTH;

    if (player.GetCursor())
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Point points[5] = {
            {x_pos, y_pos},
            {x_pos + BLOCK_WIDTH, y_pos},
            {x_pos + BLOCK_WIDTH, y_pos + BLOCK_WIDTH},
            {x_pos, y_pos + BLOCK_WIDTH},
            {x_pos, y_pos}
        };
        for (size_t i = 0; i < 5; i++)
        {
            vector2 point = {points[i].x, points[i].y};
            SDL_Point new_point = {static_cast<int>(WorldToScreen(point, camera).x), static_cast<int>(WorldToScreen(point, camera).y)};
            points[i] = new_point;
        }
        
        SDL_RenderDrawLines(renderer, points, 5);
    }
}

*/

void GameRenderer::RenderChunk(std::vector<SDL_Texture*>* textures, Chunk* chunk, Camera camera, bool cursorView)
{
    // Geometry Renderer
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;
    vector2 chunk_offset = {static_cast<float>(chunk->GetPosition().x) * BLOCK_WIDTH * CHUNK_WIDTH, static_cast<float>(chunk->GetPosition().y) * BLOCK_WIDTH * CHUNK_WIDTH};
    float r = BLOCK_WIDTH * camera.zoomMult;

    for (int i = 0; i < CHUNK_LENGTH; i++)
    {
        int block = chunk->GetBlock(i);

        // Do not render empty space
        if (block == -1)
        {
            continue;
        }

        // Get essential data from the object
        vector2 block_position = {
            (BLOCK_WIDTH * (i % CHUNK_WIDTH)),
            (BLOCK_WIDTH * floor(i / CHUNK_WIDTH))
        };
        vector2 block_position_world = WorldToScreen(block_position + chunk_offset, camera);
        float x = block_position_world.x;
        float y = block_position_world.y;

        // Get different colors for each corner, so light fades on a gradient
        SDL_Color top_right_color, bottom_right_color, bottom_left_color, top_left_color;
        std::pair<int, int> top_left_light = chunk->TryGetLight(i, {-1, -1});
        std::pair<int, int> top_middle_light = chunk->TryGetLight(i, {0, -1});
        std::pair<int, int> top_right_light = chunk->TryGetLight(i, {1, -1});
        std::pair<int, int> center_left_light = chunk->TryGetLight(i, {-1, 0});
        std::pair<int, int> center_middle_light = chunk->TryGetLight(i, {0, 0});
        std::pair<int, int> center_right_light = chunk->TryGetLight(i, {1, 0});
        std::pair<int, int> bottom_left_light = chunk->TryGetLight(i, {-1, 1});
        std::pair<int, int> bottom_middle_light = chunk->TryGetLight(i, {0, 1});
        std::pair<int, int> bottom_right_light = chunk->TryGetLight(i, {1, 1});
        float sky_light_level;
        float block_light_level;
        Uint8 light_data;

        // Top right
        sky_light_level = (top_middle_light.first + top_right_light.first + center_middle_light.first + center_right_light.first) / 4;
        block_light_level = (top_middle_light.second + top_right_light.second + center_middle_light.second + center_right_light.second) / 4;
        light_data = (Uint8)std::max({int(floor(sky_light_level * sky_light)), int(block_light_level)});
        top_right_color = {light_data, light_data, light_data, 255};

        // Bottom right
        sky_light_level = (center_middle_light.first + center_right_light.first + bottom_middle_light.first + bottom_right_light.first) / 4;
        block_light_level = (center_middle_light.second + center_right_light.second + bottom_middle_light.second + bottom_right_light.second) / 4;
        light_data = (Uint8)std::max({int(floor(sky_light_level * sky_light)), int(block_light_level)});
        bottom_right_color = {light_data, light_data, light_data, 255};


        // Bottom left
        sky_light_level = (center_left_light.first + center_middle_light.first + bottom_left_light.first + bottom_middle_light.first) / 4;
        block_light_level = (center_left_light.second + center_middle_light.second + bottom_left_light.second + bottom_middle_light.second) / 4;
        light_data = (Uint8)std::max({int(floor(sky_light_level * sky_light)), int(block_light_level)});
        bottom_left_color = {light_data, light_data, light_data, 255};


        // Top left
        sky_light_level = (top_left_light.first + top_middle_light.first + center_left_light.first + center_middle_light.first) / 4;
        block_light_level = (top_left_light.second + top_middle_light.second + center_left_light.second + center_middle_light.second) / 4;
        light_data = (Uint8)std::max({int(floor(sky_light_level * sky_light)), int(block_light_level)});
        top_left_color = {light_data, light_data, light_data, 255};

        // Atlas width = 16, make sure to update these figures if the atlas ever changes
        int atlas_x = block % 32;
        int atlas_y = floor(block / 32);

        // texture height = 16, make sure to update these figures if the atlas ever changes
        // 1 / 16 = 0.0625
        // Texture atlas width == 32, make sure to update these figures if the atlas ever changes
        // 1 / 32 = 0.03125
        float tran_x = 0.03125f;
        float tran_y = 0.0625f;
        float texture_x = tran_x * atlas_x;
        float texture_y = tran_y * atlas_y;

        // Define four corners of the quad
        float block_light = std::max({(int)floor(chunk->GetSkyLight(i) * sky_light), (int)floor(chunk->GetBlockLight(i))});
        Uint8 block_light_int = floor(block_light);
        SDL_Color block_light_color = {block_light_int, block_light_int, block_light_int, 255};

        // SDL_Vertex v1 = {{x, y}, block_light_color, {static_cast<float>(texture_x), static_cast<float>(texture_y)}};
        // SDL_Vertex v2 = {{x + r, y}, block_light_color, {static_cast<float>(texture_x + 0.03125), static_cast<float>(texture_y)}};
        // SDL_Vertex v3 = {{x + r, y + r}, block_light_color, {static_cast<float>(texture_x + 0.03125), static_cast<float>(texture_y + 0.0625)}};
        // SDL_Vertex v4 = {{x, y + r}, block_light_color, {static_cast<float>(texture_x), static_cast<float>(texture_y + 0.0625)}};
        
        SDL_Vertex v1 = {{x, y}, top_left_color, {static_cast<float>(texture_x), static_cast<float>(texture_y)}};
        SDL_Vertex v2 = {{x + r, y}, top_right_color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y)}};
        SDL_Vertex v3 = {{x + r, y + r}, bottom_right_color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y + tran_y)}};
        SDL_Vertex v4 = {{x, y + r}, bottom_left_color, {static_cast<float>(texture_x), static_cast<float>(texture_y + tran_y)}};
        
        int baseIndex = vertices.size();
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);

        // Define two triangles (quad)
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
    
    // SDL_SetRenderDrawBlendMode
    SDL_RenderGeometry(renderer, textures->at(3), vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
}

void GameRenderer::RenderEverything(std::vector<SDL_Texture*>* textures, ChunkEngine& engine, Camera camera)
{
    // Debug
    // std::cout << "Debug: beginning new frame." << std::endl;

    // Vars
    // bool cursorView = player.GetCursor();
    bool cursorView = false;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render background
    SDL_Rect background_size = {0, 0, camera.displayWidth, camera.displayHeight};
    SDL_RenderCopy(renderer, textures->at(0), NULL, &background_size);
    SDL_SetTextureAlphaMod(textures->at(1), 255 - (int)(255 * sky_light));
    SDL_SetTextureBlendMode(textures->at(1), SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, textures->at(1), NULL, &background_size);

    // Render Chunks
    for (const auto& [coord, chunkPtr] : engine.GetChunks())
    {
        RenderChunk(textures, chunkPtr.get(), camera, cursorView);
    }

    // Render player
    // RenderPlayer(textures->at(3), player, size, camera);
    
    // Present the renderer
    SDL_RenderPresent(renderer);
}
