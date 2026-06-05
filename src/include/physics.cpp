
#include "physics.hpp"

void UpdatePlayerPhysics(Player& player, ChunkEngine& engine, float dt, const uint8_t* keystates)
{
    vector2 collide;
    Collider& collider = player.GetCollider();
    // std::cout << "Debug: collider length = " << collider->size() << std::endl;

    player.UpdateInputX(dt, keystates);
    for (int i = 0; i < collider.right.size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider.right.at(i));
        if (collide.x < 0)
        {
            player.UpdateCollisionX(collide.x);
        }
    }
    for (int i = 0; i < collider.left.size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider.left.at(i));
        if (collide.x > 0)
        {
            player.UpdateCollisionX(collide.x);
        }
    }
    
    player.UpdateInputY(dt, keystates);
    for (int i = 0; i < collider.top.size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider.top.at(i));
        if (collide.y > 0)
        {
            player.UpdateCollisionY(collide.y);
        }
    }
    for (int i = 0; i < collider.bottom.size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider.bottom.at(i));
        if (collide.y < 0)
        {
            player.UpdateCollisionY(collide.y);
        }
    }
}