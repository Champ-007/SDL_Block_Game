
#include "physics.hpp"

void UpdatePlayerPhysics(Player& player, ChunkEngine& engine, float dt, const uint8_t* keystates)
{
    vector2 collide;
    std::vector<vector2>* collider = player.getCollider();
    // std::cout << "Debug: collider length = " << collider->size() << std::endl;

    player.UpdateInputX(dt, keystates);
    for (int i = 0; i < collider->size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider->at(i));
        player.UpdateCollisionX(collide.x);
    }
    
    player.UpdateInputY(dt, keystates);
    for (int i = 0; i < collider->size(); i++)
    {
        collide = engine.CollidePoint(player.GetPosition() + collider->at(i));
        player.UpdateCollisionY(collide.y);
    }
}