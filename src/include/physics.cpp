
#include "physics.hpp"

namespace Physics
{
    void UpdatePlayer(Player& player, ChunkEngine& engine, float dt, const uint8_t* keystates)
    {
        // dt = std::min(20.0f, dt);
        vector2 collide;
        Collider& collider = player.GetCollider();

        // Player platforming
        player.UpdateInputY(dt, keystates);
        for (size_t i = 0; i < collider.top.size(); i++)
        {
            collide = engine.CollidePoint(player.GetPosition() + collider.top.at(i));
            if (collide.y > 0) player.UpdateCollisionY(collide.y);
        }
        for (size_t i = 0; i < collider.bottom.size(); i++)
        {
            collide = engine.CollidePoint(player.GetPosition() + collider.bottom.at(i));
            if (collide.y < 0) player.UpdateCollisionY(collide.y);
        }

        player.UpdateInputX(dt, keystates);
        for (size_t i = 0; i < collider.right.size(); i++)
        {
            collide = engine.CollidePoint(player.GetPosition() + collider.right.at(i));
            if (collide.x < 0) player.UpdateCollisionX(collide.x);
        }
        for (size_t i = 0; i < collider.left.size(); i++)
        {
            collide = engine.CollidePoint(player.GetPosition() + collider.left.at(i));
            if (collide.x > 0) player.UpdateCollisionX(collide.x);
        }
    
        // Player controls
        player.UpdateInput(dt, keystates);

        // Player mining
        if (player.IsMining())
        {
            std::pair<bool, BlockID> result = engine.MineBlock(player.GetCursorPosition(), player.GetMining());
            player.ResetMining(result.first);
            if (result.first)
            {
                player.PickUpItem(result.second, 1);
            }
        }

        // Player Building
        if (player.IsBuilding())
        {
            if(!player.GetInventory().empty())
            {
                auto inventory = player.GetInventory().begin();
                for (int i = 0; i < player.GetInventorySelect(); i++) inventory++;
                bool result = engine.BuildBlock(player.GetCursorPosition(), inventory->first);
                if (result)
                {
                    inventory->second -= 1;
                    if (inventory->second == 0)
                    {
                        player.GetInventory().erase(inventory);
                        player.ResetInventorySelect();
                    }
                }
            }
        }
    }
}
