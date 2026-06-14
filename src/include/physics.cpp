
#include "physics.hpp"

namespace Physics
{
    void UpdatePlayer(Player& player, ChunkEngine& engine, float dt, const uint8_t* keystates)
    {
        vector2 collide;
        Collider& collider = player.GetCollider();
        // std::cout << "Debug: collider length = " << collider->size() << std::endl;

        // Determine how many sub-steps to do based on dt
        // int steps; // 

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

        // // Sub-step constants
        // const float maxStepDt = 1.0f / 30.0f;
        // // const int   maxSteps  = 128;

        // int steps = static_cast<int>(std::ceil(dt / maxStepDt));
        // std::cout << "Debug: desire steps = " << steps << std::endl;
        // steps = std::max(1, steps);

        // float subDt = dt / static_cast<float>(steps);

        // // --- Movement + collision, repeated per sub-step ---
        // for (int step = 0; step < steps; step++)
        // {
        //     // Player platforming - X axis
        // }
    
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
