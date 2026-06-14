
#include "player.hpp"

Player::Player(vector2 _pos, float _w, float _h)
{
    position = _pos;
    width = _w;
    height = _h;
    velocity = {0, 0};
    playerCollider.Build(_w, _h);
    inventory.clear();
}

vector2 Player::GetPosition()
{
    return position;
}
void    Player::SetPosition(vector2 p)
{
    position = p;
}

vector2 Player::GetSize()
{
    return {width, height};
}
void    Player::SetSize(vector2 s)
{
    width = s.x;
    height = s.y;
    // playerCollider.UpdateCollider(width, height);
}

float Player::GetWidth()
{
    return width;
}
void  Player::SetWidth(float w)
{
    width = w;
    playerCollider.Build(width, height);
}

float Player::GetHeight()
{
    return height;
}
void  Player::SetHeight(float h)
{
    height = h;
    playerCollider.Build(width, height);
}

Collider& Player::GetCollider()
{
    return playerCollider;
}

void Player::UpdateCollisionX(float depth)
{
    bool opposite = (depth > 0 && velocity.x < 0) || (depth < 0 && velocity.x > 0);
    if (depth != 0 && opposite)
    {
        position.x += depth;
        // std::cout << "position x = " << position.x << std::endl;
        velocity.x = 0.0f;
    }
    // my_depth.x = depth;
}

void Player::UpdateCollisionY(float depth)
{
    bool opposite = (depth > 0 && velocity.y < 0) || (depth < 0 && velocity.y > 0);
    if (depth != 0 && opposite)
    {
        position.y += depth;
        velocity.y = 0.0f;
        if (depth < 0.0f)
        {
            falling = 0;
        }
    }
    // my_depth.y = depth;
}

void Player::UpdateCollision(vector2 col)
{
    // Debug
    // std::cout << "Debug: collision = (" << col.top << ", " << col.right << ", " << col.bottom << ", " << col.left << ")" << std::endl; 

    // If both top and bottom are colliding, then ignore vertical collision
    if (col.y != 0)
    {
        std::cout << "Debug: fix y = " << col.y << std::endl;
        position.y += col.y;
        velocity.y = 0.0f;
        if (col.y < 0.0f)
        {
            falling = 0;
        }
    }

    // If both right and left are colliding, then ignore horizontal collision
    if (col.x != 0)
    {
        position.x += col.x;
        velocity.x = 0.0f;
    }
}

bool Player::GetCursor()
{
    return crouch == 1;
}

vector2 Player::GetCursorPosition()
{
    return cursor_position;
}

bool Player::IsMining()
{
    return mining > 0.0f && tool == 1;
}

bool Player::IsBuilding()
{
    return mining == 1.0f && tool == 0;
}

float Player::GetMining()
{
    return mining;
}

void Player::ResetMining(bool t)
{
    if (t) mining = 0.0f;
}

void Player::PickUpItem(BlockID id, int count)
{
    if (count > 1) PickUpItem(id, count - 1);

    // Add item to nearest open stack
    for (auto& item : inventory)
    {
        if (item.first == id && item.second < inventoryStackMax)
        {
            item.second += 1;
            return;
        }
    }

    // Try to create new inventory slot item
    if (inventory.size() < inventorySlotsMax)
    {
        inventory.push_back({id, 1});
        return;
    }

    // Defaulting. Right now a maxed inventory means items are burned to the void.

    // Debug inventory
    // int slot = 1;
    // for (auto item : inventory)
    // {
    //     std::cout << "Inventory slot " << slot << ": " << item.first << ", " << item.second << std::endl;
    //     slot++;
    // }
    // std::cout << "---" << std::endl;
}

void Player::UpdateInputY(float dt, const uint8_t* keystates)
{
    // Vertical movement
    if (crouch == 0)
    {
        if (keystates[SDL_SCANCODE_SPACE] && falling <= 3)
        {
            velocity.y -= jumpForce;
            falling = 4;
        }
    }
    velocity.y += gravity * dt;
    falling += 1;
    
    position.y += velocity.y * dt;
}

void Player::UpdateInputX(float dt, const uint8_t* keystates)
{
    // Horisontal movement
    float right = float(keystates[SDL_SCANCODE_D]);
    float left  = float(keystates[SDL_SCANCODE_A]);
    float move_x = right - left;
    if (crouch == 0)
    {
        velocity.x += move_x * moveSpeed * dt;
    }
    velocity.x *= std::pow(1.0f - moveFriction, dt);

    position.x += velocity.x * dt;

}

void Player::UpdateInput(float dt, const uint8_t* keystates)
{
    // Cursor Movement
    if (crouch == 1)
    {
        // Move cursor left
        if (keystates[SDL_SCANCODE_A] && !curTrig_a)
        {
            vector2 move = {-1, 0};
            cursor_position += move;
            mining = 0.0f;
            curTrig_a = true;
        }
        if (!keystates[SDL_SCANCODE_A] && curTrig_a)
        {
            curTrig_a = false;
        }

        // Move cursor right
        if (keystates[SDL_SCANCODE_D] && !curTrig_d)
        {
            vector2 move = {1, 0};
            cursor_position += move;
            mining = 0.0f;
            curTrig_d = true;
        }
        if (!keystates[SDL_SCANCODE_D] && curTrig_d)
        {
            curTrig_d = false;
        }

        // Move cursor up
        if (keystates[SDL_SCANCODE_W] && !curTrig_w)
        {
            vector2 move = {0, -1};
            cursor_position += move;
            mining = 0.0f;
            curTrig_w = true;
        }
        if (!keystates[SDL_SCANCODE_W] && curTrig_w)
        {
            curTrig_w = false;
        }

        // Move cursor down
        if (keystates[SDL_SCANCODE_S] && !curTrig_s)
        {
            vector2 move = {0, 1};
            cursor_position += move;
            mining = 0.0f;
            curTrig_s = true;
        }
        if (!keystates[SDL_SCANCODE_S] && curTrig_s)
        {
            curTrig_s = false;
        }

        // Mine
        if (keystates[SDL_SCANCODE_LEFT] && tool == 1)
        {
            mining += mineSpeed * dt;
        }

        // Build
        if (keystates[SDL_SCANCODE_LEFT] && tool == 0)
        {
            if (!buildTrig)
            {
                mining = 1.0f;
                buildTrig = true;
            }
            else mining = 0.0f;
        }

        if (!keystates[SDL_SCANCODE_LEFT])
        {
            mining = 0.0f;
            buildTrig = false;
        }
    }

    // Switch Tool
    if (keystates[SDL_SCANCODE_Q])
    {
        tool = 0;
        ResetMining(true);
        // std::cout << "Debug: switching to build tool" << std::endl;
    }
    if (keystates[SDL_SCANCODE_E])
    {
        tool = 1;
        ResetMining(true);
        // std::cout << "Debug: switching to mining tool" << std::endl;
    }

    // Change Inventory slot
    if (keystates[SDL_SCANCODE_UP] && selectChangeTrig == false)
    {
        select += 1;
        if (select >= inventory.size() || select < 0)
        {
            select = 0;
        }
        selectChangeTrig = true;
    }
    else if (!keystates[SDL_SCANCODE_UP] && true)
    {
        selectChangeTrig = false;
    }

    // Crouching
    if (keystates[SDL_SCANCODE_DOWN] && height > 20)
    {
        SetHeight(15);
        crouch = 1;
        cursor_position = {floor(position.x / BLOCK_WIDTH), floor(position.y / BLOCK_WIDTH)};
    }
    if (!keystates[SDL_SCANCODE_DOWN] && height < 20)
    {
        SetHeight(28);
        crouch = 0;
    }

    // position += velocity * dt;

}

std::list<std::pair<BlockID, unsigned int>>& Player::GetInventory()
{
    return inventory;
}

int Player::GetInventorySelect()
{
    return select;
}

void Player::ResetInventorySelect()
{
    select = 0;
}

bool Player::GetVelDir()
{
    return (abs(velocity.y) >= abs(velocity.x));
}

// void Player::Update(float dt)
// {

// }

// vector2 Player::GetDepth()
// {
//     return my_depth;
// }

