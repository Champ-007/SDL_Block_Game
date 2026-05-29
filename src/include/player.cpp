
#include "player.hpp"

Player::Player(vector2 _pos, float _w, float _h)
{
    position = _pos;
    width = _w;
    height = _h;
    velocity = {0, 0};
    // playerCollider.UpdateCollider(_w, _h);
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
    // playerCollider.UpdateCollider(width, height);
}

float Player::GetHeight()
{
    return height;
}
void  Player::SetHeight(float h)
{
    height = h;
    // playerCollider.UpdateCollider(width, height);
}

// Collider& Player::GetCollider()
// {
//     return playerCollider;
// }

// void Player::UpdateCollisionX(float depth)
// {
//     if (depth != 0)
//     {
//         position += depth;
//         std::cout << "position x = " << position.x << std::endl;
//         velocity.x = 0.0f;
//     }
//     my_depth.x = depth;
// }

// void Player::UpdateCollisionY(float depth)
// {
//     if (depth != 0)
//     {
//         position.y += depth;
//         velocity.y = 0.0f;
//         if (depth < 0.0f)
//         {
//             falling = 0;
//         }
//     }
//     my_depth.y = depth;
// }

// void Player::UpdateCollision(vector2 col)
// {
//     // Debug
//     // std::cout << "Debug: collision = (" << col.top << ", " << col.right << ", " << col.bottom << ", " << col.left << ")" << std::endl; 

//     // If both top and bottom are colliding, then ignore vertical collision
//     if (col.y != 0)
//     {
//         std::cout << "Debug: fix y = " << col.y << std::endl;
//         position.y += col.y;
//         velocity.y = 0.0f;
//         if (col.y < 0.0f)
//         {
//             falling = 0;
//         }
//     }

//     // If both right and left are colliding, then ignore horizontal collision
//     if (col.x != 0)
//     {
//         position.x += col.x;
//         velocity.x = 0.0f;
//     }
// }

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
    return mining == 1;
}

void Player::UpdateInputY(float dt, const uint8_t* keystates)
{
    // Vertical movement
    // if (crouch == 0)
    // {
    //     if (keystates[SDL_SCANCODE_SPACE] && falling < 4)
    //     {
    //         velocity.y = -jumpForce;
    //     }
    // }
    // velocity.y += gravity * dt;
    // falling += 1;
    float down = float(keystates[SDL_SCANCODE_S]);
    float up  = float(keystates[SDL_SCANCODE_W]);
    float move_y = down - up;
    velocity.y += move_y * moveSpeed;
    velocity.y *= 0.8f;

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
        velocity.x += move_x * moveSpeed;
    }
    velocity.x *= 0.8f;

    position.x += velocity.x * dt;

}

void Player::UpdateInput(float dt, const uint8_t* keystates)
{
    // keystates = _keystates;

    // Vertical movement
    // if (crouch == 0)
    // {
    //     if (keystates[SDL_SCANCODE_SPACE] && falling < 4)
    //     {
    //         velocity.y = -jumpForce;
    //     }
    // }
    // velocity.y += gravity * dt;
    // falling += 1;
    
    // Horisontal movement
    // float right = (int)keystates[SDL_SCANCODE_D];
    // float left  = (int)keystates[SDL_SCANCODE_A];
    // float move_x = right - left;
    // if (crouch == 0)
    // {
    //     velocity.x += move_x * moveSpeed;
    // }
    // velocity.x *= 0.8f;

    // Cursor Movement
    mining = 0;
    if (crouch == 1)
    {
        // Move cursor left
        if (keystates[SDL_SCANCODE_A] && !curTrig_a)
        {
            vector2 move = {-1, 0};
            cursor_position += move;
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
            curTrig_s = true;
        }
        if (!keystates[SDL_SCANCODE_S] && curTrig_s)
        {
            curTrig_s = false;
        }

        // Mine
        if (keystates[SDL_SCANCODE_LEFT] && !mineTrig)
        {
            mining = 1;
        }
        mineTrig = keystates[SDL_SCANCODE_LEFT];
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

// void Player::Update(float dt)
// {

// }

// vector2 Player::GetDepth()
// {
//     return my_depth;
// }

