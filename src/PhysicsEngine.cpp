#include "../include/bettlerider/PhysicsEngine.h"

namespace BRTC
{
    bool PhysicsEngine::CheckCollision(const Object& obj1, const Object& obj2) 
    {
        const Vector pos1 = obj1.getPosition();
        const Vector size1 = obj1.getSize();
        Vector pos2 = obj2.getPosition();
        Vector size2 = obj2.getSize();
        return 
        (
            pos1.x < pos2.x + size2.x &&
            pos1.x + size1.x > pos2.x &&
            pos1.y < pos2.y + size2.y &&
            pos1.y + size1.y > pos2.y
        );
    }

    void PhysicsEngine::HandleCollisions
    (
        DynamicObject& dynamicObject,
        const std::list<Wall>& walls,
        const std::list<Platform>& platforms,
        const std::list<SolidPlatform>& solidPlatforms
    ) 
    {
        Vector position = dynamicObject.getPosition();
        const Vector size = dynamicObject.getSize();
        Vector velocity = dynamicObject.getVelocity();
        bool hasPlatformCollision = false;
        for (const auto& wall : walls) 
        {
            if (!CheckCollision(dynamicObject, wall)) continue;
            const Vector wallPos = wall.getPosition();
            const Vector wallSize = wall.getSize();
            if (velocity.x > 0) { position.x = wallPos.x - size.x; } 
            else if (velocity.x < 0) { position.x = wallPos.x + wallSize.x;}
            velocity.x = 0;   
        }
        for (const auto& platform : solidPlatforms) {
            if (!CheckCollision(dynamicObject, platform)) continue;
            hasPlatformCollision = true;
            const Vector platformPos = platform.getPosition();
            const Vector platformSize = platform.getSize();    
            if (velocity.y > 0) 
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);
                dynamicObject.setFalling(false);    
            } 
            else if (velocity.y < 0) 
            {
                position.y = platformPos.y + platformSize.y;
                velocity.y = 0;
                dynamicObject.setFalling(true);
            }
        }
        if (!dynamicObject.isPassingThroughPlatform()) 
        {
        for (const auto& platform : platforms) 
            {
            if (!CheckCollision(dynamicObject, platform)) continue;
            hasPlatformCollision = true;
            const Vector platformPos = platform.getPosition();
            const Vector platformSize = platform.getSize();    
            if (velocity.y >= 0) 
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);
            }
            }
        }
        if (!hasPlatformCollision  && dynamicObject.isOnGround()) 
        {
            dynamicObject.setOnGround(false);
            dynamicObject.setFalling(true);
        }
        dynamicObject.setPosition(position);
        dynamicObject.setVelocity(velocity);
    }
    bool PhysicsEngine::HandlePlayerCollisions
    (
        Player& player,
        Vector& spawnPosition,
        const std::list<Door>& doors,
        std::list<Crate>& crates,
        std::string& levelToLoad
    ) 
    {
        for (auto& door : doors) 
        {
            if (!CheckCollision(player, door)) continue;
            levelToLoad = door.getLevelToLoad();
            if (door.hasValidSpawn()) { spawnPosition = door.getSpawnPosition(); } 
            else { spawnPosition = Vector(-1, -1); }
            return true;
        }
        bool wasOnGround = player.isOnGround();
        bool standingOnCrate = false;
        for (auto& crate : crates) 
        {
            if (!CheckCollision(player, crate)) continue;
            const Vector playerPos = player.getPosition();
            const Vector playerSize = player.getSize();
            const Vector cratePos = crate.getPosition();
            const Vector crateSize = crate.getSize();
            const float playerBottom = playerPos.y + playerSize.y;
            const float crateTop = cratePos.y;
            const float overlapVertical = playerBottom - crateTop;    
            const float playerRight = playerPos.x + playerSize.x;
            const float crateLeft = cratePos.x;
            const float playerLeft = playerPos.x;
            const float crateRight = cratePos.x + crateSize.x;
            const float overlapHorizontal = std::min(playerRight - crateLeft, crateRight - playerLeft);    
            if (overlapVertical < overlapHorizontal) 
            {
                if (player.getVelocity().y > 0) 
                {
                    player.setPosition(Vector(playerPos.x, crateTop - playerSize.y));
                    player.setVelocity(Vector(player.getVelocity().x, 0));
                    standingOnCrate = true;
                    wasOnGround = true;
                }
            }
            else 
            {
                const Vector pushForce(player.getVelocity().x * 1.2f, 0.0f);
                crate.applyForce(pushForce);
                    
                if (player.getVelocity().x > 0) 
                { player.setPosition(Vector(crateLeft - playerSize.x, playerPos.y)); } 
                else if (player.getVelocity().x < 0) { player.setPosition(Vector(crateRight, playerPos.y)); }
            }
        }
        player.setOnGround(wasOnGround || standingOnCrate);
        return false;
    }
} // namespace BRTC