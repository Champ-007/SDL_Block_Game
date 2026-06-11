
#include <renderer.hpp>

GameRenderer::GameRenderer(SDL_Renderer* _renderer)
{
    renderer = _renderer;
    camera = Camera({0, 0}, 4.0f);
}

Camera* GameRenderer::GetCamera()
{
    return &camera;
}

void GameRenderer::UpdateCamera(vector2 position, float dt)
{
    // Camera tracks position
    vector2 target = {position.x - (camera.displayWidth / 2 / camera.zoomMult), position.y - (camera.displayHeight / 2 / camera.zoomMult)};
    vector2 diff = target - camera.position;
    camera.position += diff * 0.2f * dt;

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

SDL_Color GameRenderer::MultBrightness(SDL_Color color, float v)
{
    return {static_cast<Uint8>(color.r * v), static_cast<Uint8>(color.g * v), static_cast<Uint8>(color.b * v), color.a};
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

vector2 GameRenderer::WorldToScreen(vector2 position)
{
    return (position - camera.position) * camera.zoomMult;
}

// vector2 GameRenderer::ScreenToWorld(vector2 position, Camera camera)
// {
//     return {
//         (position.x - (camera.displayWidth  / 2)) / camera.zoomMult + (camera.displayWidth  / 2) + camera.position.x,
//         (position.y - (camera.displayHeight / 2)) / camera.zoomMult + (camera.displayHeight / 2) + camera.position.y
//     };
// }

vector2 GameRenderer::ScreenToWorld(vector2 position)
{
    return {
        (position.x - (camera.displayWidth  / 2)) / camera.zoomMult + camera.position.x,
        (position.y - (camera.displayHeight / 2)) / camera.zoomMult + camera.position.y
    };
}

void GameRenderer::RenderPlayer(SDL_Texture* texture, Player& player, ChunkEngine* engine)
{
    SDL_Point size = {int(player.GetWidth()), int(player.GetHeight())};
    vector2 position = player.GetPosition();

    vector2 position_block = {
        floor(position.x / BLOCK_WIDTH),
        floor(position.y / BLOCK_WIDTH)
    };
    std::pair<int, int> light = engine->GetLight(position_block + vector2(0, -1));
    light.first = static_cast<int>(light.first * sky_light);
    int render_light = std::max(light.first, light.second);
    light = engine->GetLight(position_block);
    light.first = static_cast<int>(light.first * sky_light);
    render_light = (render_light + std::max(light.first, light.second)) / 2;

    SDL_SetTextureColorMod(texture, render_light, render_light, render_light);


    vector2 screenPosition = WorldToScreen(position);
    SDL_FRect frect = {screenPosition.x, screenPosition.y, static_cast<float>(size.x) * camera.zoomMult, static_cast<float>(size.y) * camera.zoomMult};
    SDL_FRectCenter(frect);
    SDL_RenderCopyF(renderer, texture, NULL, &frect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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
            SDL_Point new_point = {static_cast<int>(WorldToScreen(point).x), static_cast<int>(WorldToScreen(point).y)};
            points[i] = new_point;
        }
        
        SDL_RenderDrawLines(renderer, points, 5);
    }
}

void GameRenderer::RenderChunk(std::vector<SDL_Texture*>* textures, Chunk* chunk, bool cursorView)
{
    // Determine if Chunk is visible
    vector2 nearScreenCorner = ScreenToWorld({0, 0});
    vector2 farScreenCorner  = ScreenToWorld({camera.displayWidth * 2, camera.displayHeight * 2});
    vector2 chunk_world = {static_cast<float>(chunk->GetPosition().x) * BLOCK_WIDTH * CHUNK_WIDTH, static_cast<float>(chunk->GetPosition().y) * BLOCK_WIDTH * CHUNK_WIDTH};
    float chunk_worldWidth = BLOCK_WIDTH * CHUNK_WIDTH;
    float r = BLOCK_WIDTH * camera.zoomMult;

    bool visible = true;
    if (chunk_world.x > farScreenCorner.x) visible = false;
    if (chunk_world.x + chunk_worldWidth < nearScreenCorner.x) visible = false;
    
    if (chunk_world.y > farScreenCorner.y) visible = false;
    if (chunk_world.y + chunk_worldWidth < nearScreenCorner.y) visible = false;

    if (visible)
    {
        // Geometry Renderer
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;
    
        for (int i = 0; i < CHUNK_LENGTH; i++)
        {
            BlockID block = chunk->GetBlock(i);
            // std::cout << "Debug: Starting chunk render." << std::endl;
            
            const BlockDef& def = BlockRegistry::get(block);
    
            // Get essential data from the object
            vector2 block_position = {
                (BLOCK_WIDTH * (i % CHUNK_WIDTH)),
                (BLOCK_WIDTH * floor(i / CHUNK_WIDTH))
            };
            vector2 block_position_world = WorldToScreen(block_position + chunk_world);
            float x = block_position_world.x;
            float y = block_position_world.y;
    
            // Get different colors for each corner, so light fades on a gradient
            SDL_Color top_right_color, bottom_right_color, bottom_left_color, top_left_color;
            std::pair<int, int> top_left_light = chunk->TryGetLight(i, {-1, -1});
            std::pair<int, int> top_middle_light = chunk->TryGetLight(i, {0, -1});
            std::pair<int, int> top_right_light = chunk->TryGetLight(i, {1, -1});
            std::pair<int, int> center_left_light = chunk->TryGetLight(i, {-1, 0});
            std::pair<int, int> center_middle_light = chunk->GetLight(i);
            std::pair<int, int> center_right_light = chunk->TryGetLight(i, {1, 0});
            std::pair<int, int> bottom_left_light = chunk->TryGetLight(i, {-1, 1});
            std::pair<int, int> bottom_middle_light = chunk->TryGetLight(i, {0, 1});
            std::pair<int, int> bottom_right_light = chunk->TryGetLight(i, {1, 1});
            float sky_light_level;
            float block_light_level;
            Uint8 light_data;
    
            // /*
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
            // */
    
            // Solid Light
            float block_light = std::max({(int)floor(chunk->GetSkyLight(i) * sky_light), (int)floor(chunk->GetBlockLight(i))});
            Uint8 block_light_int = floor(block_light);
            SDL_Color block_light_color = {block_light_int, block_light_int, block_light_int, 255};
            
            // Atlas width = 32, make sure to update these figures if the atlas ever changes
            // block UVs
            int atlas_x = def.textureIndex % 32;
            int atlas_y = floor(def.textureIndex / 32);
    
            // background UVs
            int background_x = 0;
            int background_y = 0;
            if (chunk->GetBiome() == -1)
            {
                background_x = 19;
                background_y = 10;
            }
            else if (chunk->GetBiome() == 1)
            {
                background_x = 20;
                background_y = 3;
            }
    
            // texture height = 16, make sure to update these figures if the atlas ever changes
            // 1 / 16 = 0.0625
            // Texture atlas width == 32, make sure to update these figures if the atlas ever changes
            // 1 / 32 = 0.03125
            float tran_x = 0.03125f;
            float tran_y = 0.0625f;
            float texture_x = tran_x * atlas_x;
            float texture_y = tran_y * atlas_y;
            float background_texture_x = tran_x * background_x;
            float background_texture_y = tran_y * background_y;
    
            // Put another quad behind the block's quad
            if (def.isTransparent && chunk->GetBiome() != 0)
            {
                // Define four corners of the quad
                float mult = 0.7f;
                // SDL_Vertex v1 = {{x, y}, MultBrightness(block_light_color, mult), {static_cast<float>(background_texture_x), static_cast<float>(background_texture_y)}};
                // SDL_Vertex v2 = {{x + r, y}, MultBrightness(block_light_color, mult), {static_cast<float>(background_texture_x + tran_x), static_cast<float>(background_texture_y)}};
                // SDL_Vertex v3 = {{x + r, y + r}, MultBrightness(block_light_color, mult), {static_cast<float>(background_texture_x + tran_x), static_cast<float>(background_texture_y + tran_y)}};
                // SDL_Vertex v4 = {{x, y + r}, MultBrightness(block_light_color, mult), {static_cast<float>(background_texture_x), static_cast<float>(background_texture_y + tran_y)}};
                
                SDL_Vertex v1 = {{x, y}, MultBrightness(top_left_color, mult), {static_cast<float>(background_texture_x), static_cast<float>(background_texture_y)}};
                SDL_Vertex v2 = {{x + r, y}, MultBrightness(top_right_color, mult), {static_cast<float>(background_texture_x + tran_x), static_cast<float>(background_texture_y)}};
                SDL_Vertex v3 = {{x + r, y + r}, MultBrightness(bottom_right_color, mult), {static_cast<float>(background_texture_x + tran_x), static_cast<float>(background_texture_y + tran_y)}};
                SDL_Vertex v4 = {{x, y + r}, MultBrightness(bottom_left_color, mult), {static_cast<float>(background_texture_x), static_cast<float>(background_texture_y + tran_y)}};
                
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
    
            // The block's quad
            if (def.name != "air")
            {
                // Define four corners of the quad
                // SDL_Vertex v1 = {{x, y}, block_light_color, {static_cast<float>(texture_x), static_cast<float>(texture_y)}};
                // SDL_Vertex v2 = {{x + r, y}, block_light_color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y)}};
                // SDL_Vertex v3 = {{x + r, y + r}, block_light_color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y + tran_y)}};
                // SDL_Vertex v4 = {{x, y + r}, block_light_color, {static_cast<float>(texture_x), static_cast<float>(texture_y + tran_y)}};
                
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
        }
        
        // SDL_SetRenderDrawBlendMode
        SDL_RenderGeometry(renderer, textures->at(3), vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
    }
}

void GameRenderer::RenderEverything(std::vector<SDL_Texture*>* textures, ChunkEngine& engine, Player& player, float dt)
{
    // Debug
    // std::cout << "Debug: beginning new frame." << std::endl;

    // Vars
    bool cursorView = player.GetCursor();
    // bool cursorView = false;

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
        RenderChunk(textures, chunkPtr.get(), cursorView);
    }

    // Render player
    RenderPlayer(textures->at(2), player, &engine);

    // Render FPS
    RenderDebug(textures, dt);

    // Render Inventory
    RenderInventory(textures, player);
    
    // Present the renderer
    SDL_RenderPresent(renderer);
}

void GameRenderer::RenderDebug(std::vector<SDL_Texture*>* textures, float dt)
{
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    if (FPSRefreshCountdown == 0)
    {
        int fps = 1000 / (dt / 0.02f);
        FPS = fps;
        FPSRefreshCountdown = 4;
    }
    else
    {
        FPSRefreshCountdown -= 1;
    }

    int letterSize = 16;

    float scale = 1.0f;

    // std::cout << "FPS: " << fps << std::endl;
    std::string str = std::to_string(FPS) + " FPS";
    SDL_FRect rect = {10, 10, letterSize * scale, letterSize * scale};
    for (char ch : str)
    {
        //std::cout << "Debug: char = " << ch << std::endl;

        int textureIndex = 0;
        switch (ch)
        {
            case '0':
                textureIndex = 0+16;
                break;
            case '1':
                textureIndex = 1+16;
                break;
            case '2':
                textureIndex = 2+16;
                break;
            case '3':
                textureIndex = 3+16;
                break;
            case '4':
                textureIndex = 4+16;
                break;
            case '5':
                textureIndex = 5+16;
                break;
            case '6':
                textureIndex = 6+16;
                break;
            case '7':
                textureIndex = 7+16;
                break;
            case '8':
                textureIndex = 8+16;
                break;
            case '9':
                textureIndex = 9+16;
                break;
            case 'F':
                textureIndex = 38;
                break;
            case 'P':
                textureIndex = 48;
                break;
            case 'S':
                textureIndex = 51;
                break;
            default:
                textureIndex = 0;
                break;
        }


        // std::cout << "Debug: textureIndex = " << textureIndex << std::endl;

        // Atlas width = 16, make sure to update these figures if the atlas ever changes
        int atlas_x = textureIndex % 16;
        int atlas_y = floor(textureIndex / 16);

        // texture height = 16, make sure to update these figures if the atlas ever changes
        // 1 / 16 = 0.0625
        // Texture atlas width = 16, make sure to update these figures if the atlas ever changes
        // 1 / 16 = 0.0625
        float tran_x = 0.0625f;
        float tran_y = 0.0625f;
        float texture_x = atlas_x * letterSize;
        float texture_y = atlas_y * letterSize;

        SDL_Rect src_rect = {static_cast<int>(texture_x), static_cast<int>(texture_y), 16, 16};

        SDL_RenderCopyF(renderer, textures->at(4), &src_rect, &rect);
        rect.x += letterSize * scale;
    }
}

void GameRenderer::RenderInventory(std::vector<SDL_Texture*>* textures, Player& player)
{
    int slotWidth = 64;
    auto inventory = player.GetInventory();
    int offset = 16;
    float iconSize = 0.7;

    // Geometry Renderer
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;
    float r = slotWidth * iconSize;

    int count = 0;
    for (auto item : inventory)
    {
        // Render White Outline
        // std::cout << "Debug: Rendering white outline" << std::endl;
        SDL_Point pos = {
            offset + (count * slotWidth),
            camera.displayHeight - (slotWidth + 16)
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Point points[5] = {
            {pos.x, pos.y},
            {pos.x + slotWidth, pos.y},
            {pos.x + slotWidth, pos.y + slotWidth},
            {pos.x, pos.y + slotWidth},
            {pos.x, pos.y}
        };

        if (count == player.GetInventorySelect())
        {
            points[1].y += -10;
        }
        
        SDL_RenderDrawLines(renderer, points, 5);

        // Add Block Item to geometry
        BlockID block = item.first;
        const BlockDef& def = BlockRegistry::get(block);
        SDL_Color color = {255, 255, 255, 255};

        // Atlas width = 32, make sure to update these figures if the atlas ever changes
        // block UVs
        int atlas_x = def.textureIndex % 32;
        int atlas_y = floor(def.textureIndex / 32);

        // texture height = 16, make sure to update these figures if the atlas ever changes
        // 1 / 16 = 0.0625
        // Texture atlas width == 32, make sure to update these figures if the atlas ever changes
        // 1 / 32 = 0.03125
        float tran_x = 0.03125f;
        float tran_y = 0.0625f;
        float texture_x = tran_x * atlas_x;
        float texture_y = tran_y * atlas_y;

        float x = pos.x + (slotWidth * (1 - iconSize) / 2);
        float y = pos.y + (slotWidth * (1 - iconSize) / 2);

        // The Quad of the Item
        if (def.name != "air")
        {    
            SDL_Vertex v1 = {{x, y}, color, {static_cast<float>(texture_x), static_cast<float>(texture_y)}};
            SDL_Vertex v2 = {{x + r, y}, color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y)}};
            SDL_Vertex v3 = {{x + r, y + r}, color, {static_cast<float>(texture_x + tran_x), static_cast<float>(texture_y + tran_y)}};
            SDL_Vertex v4 = {{x, y + r}, color, {static_cast<float>(texture_x), static_cast<float>(texture_y + tran_y)}};
            
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

        // Render the item count
        std::string str = std::to_string(item.second);
        SDL_FRect rect = {static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(slotWidth * 0.3), static_cast<float>(slotWidth * 0.3)};
        for (char ch : str)
        {
            int textureIndex = 0;
            switch (ch)
            {
                case '0':
                    textureIndex = 0+16;
                    break;
                case '1':
                    textureIndex = 1+16;
                    break;
                case '2':
                    textureIndex = 2+16;
                    break;
                case '3':
                    textureIndex = 3+16;
                    break;
                case '4':
                    textureIndex = 4+16;
                    break;
                case '5':
                    textureIndex = 5+16;
                    break;
                case '6':
                    textureIndex = 6+16;
                    break;
                case '7':
                    textureIndex = 7+16;
                    break;
                case '8':
                    textureIndex = 8+16;
                    break;
                case '9':
                    textureIndex = 9+16;
                    break;
                default:
                    textureIndex = 0;
                    break;
            }

            // Atlas width = 16, make sure to update these figures if the atlas ever changes
            int atlas_x = textureIndex % 16;
            int atlas_y = floor(textureIndex / 16);

            // texture height = 16, make sure to update these figures if the atlas ever changes
            // 1 / 16 = 0.0625
            // Texture atlas width = 16, make sure to update these figures if the atlas ever changes
            // 1 / 16 = 0.0625
            float tran_x = 0.0625f;
            float tran_y = 0.0625f;
            float texture_x = atlas_x * 16;
            float texture_y = atlas_y * 16;

            SDL_Rect src_rect = {static_cast<int>(texture_x), static_cast<int>(texture_y), 16, 16};

            SDL_RenderCopyF(renderer, textures->at(4), &src_rect, &rect);
        }
            
        count += 1;
    }

    // Render the Block icons of the inventory HUD
    SDL_RenderGeometry(renderer, textures->at(3), vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
}