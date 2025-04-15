#include "../include/bettlerider/PhysicsEngine.h"

namespace BRTC
{
    bool PhysicsEngine::CheckCollision(const Object& staticObj, const Object& dynamicObj)
   {
        const Vector staticPos = staticObj.getPosition();
        const Vector dynamicPos = dynamicObj.getPosition();
        const Vector staticSize = staticObj.getSize();
        const Vector dynamicSize = dynamicObj.getSize();
        return 
        (
            dynamicPos.x < staticPos.x + staticSize.x &&
            dynamicPos.x + dynamicSize.x > staticPos.x &&
            dynamicPos.y < staticPos.y + staticSize.y &&
            dynamicPos.y + dynamicSize.y > staticPos.y
        );
    }
    void PhysicsEngine::HandleCollisions
    (
        DynamicObject &dynamicObject,
        const std::list<Wall> &walls,
        const std::list<Platform> &platforms,
        const std::list<SolidPlatform> &solidPlatforms 
    )
    {
        Vector position = dynamicObject.getPosition();
        const Vector size = dynamicObject.getSize();
        Vector velocity = dynamicObject.getVelocity();    
        bool hasPlatformCollision = handleWallCollisions
        (
            dynamicObject, 
            walls, 
            position, 
            velocity
        );
        hasPlatformCollision |= handleSolidPlatformCollisions
        (
            dynamicObject, 
            solidPlatforms, 
            position, 
            velocity
        );
        if (!dynamicObject.isPassingThroughPlatform()) 
        {
            hasPlatformCollision |= handlePlatformCollisions
            (
                dynamicObject, 
                platforms, 
                position, 
                velocity
            );
        }
        updateGroundState(dynamicObject, hasPlatformCollision);
        dynamicObject.setPosition(position);
        dynamicObject.setVelocity(velocity);
    }

    bool PhysicsEngine::handleWallCollisions
    (
        DynamicObject &dynamicObject,
        const std::list<Wall> &walls,
        Vector& position,
        Vector& velocity
    )
    {
        bool collisionOcurred = false;
        const Vector size = dynamicObject.getSize();

        for (const auto& wall: walls) 
        {
            if (!CheckCollision(wall, dynamicObject)) continue;
            collisionOcurred = true;
            const Vector wallPos = wall.getPosition();
            const Vector wallSize = wall.getSize();
            if (velocity.x > 0)
            {
                position.x = wallPos.x - size.x;
            }
            else if (velocity.x < 0)
            {
                position.x = wallPos.x + wallSize.x;
            }
            velocity.x = 0;
        }
        return collisionOcurred;
    }

    bool PhysicsEngine::handleSolidPlatformCollisions
    (
        DynamicObject& dynamicObject,
        const std::list<SolidPlatform>& solidPlatforms,
        Vector& position,
        Vector& velocity) 
    {
        bool collisionOccurred = false;
        const Vector size = dynamicObject.getSize();    
        for (const auto& platform : solidPlatforms) 
        {
            if (!CheckCollision(dynamicObject, platform)) continue;    
            collisionOccurred = true;
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
        return collisionOccurred;
    }
    
    bool PhysicsEngine::handlePlatformCollisions
    (
        DynamicObject& dynamicObject,
        const std::list<Platform>& platforms,
        Vector& position,
        Vector& velocity
    ) 
    {
        bool collisionOccurred = false;
        const Vector size = dynamicObject.getSize();
        for (const auto& platform : platforms) 
        {
            if (!CheckCollision(dynamicObject, platform)) continue;    
            collisionOccurred = true;
            const Vector platformPos = platform.getPosition();
            const Vector platformSize = platform.getSize();
            if (velocity.y >= 0) 
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);
            }
        }
        return collisionOccurred;
    }

    void PhysicsEngine::updateGroundState
    (
        DynamicObject& dynamicObject, 
        bool hasPlatformCollision
    ) 
    {
        if (!hasPlatformCollision && dynamicObject.isOnGround()) 
        {
            dynamicObject.setOnGround(false);
            dynamicObject.setFalling(true);
        }
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
        if (checkDoorCollisions(player, doors, spawnPosition, levelToLoad)) 
        {return true;}
        handleCrateCollisions(player, crates);
        return false;
    }

    bool PhysicsEngine::checkDoorCollisions
    (
        const Player& player,
        const std::list<Door>& doors,
        Vector& spawnPosition,
        std::string& levelToLoad) 
    {
        for (const auto& door : doors) 
        {
            if (!CheckCollision(player, door)) continue;    
            levelToLoad = door.getLevelToLoad();
            spawnPosition = door.hasValidSpawn() ? door.getSpawnPosition() : Vector(-1, -1);
            return true;
        }
        return false;
    }
    
    void PhysicsEngine::handleCrateCollisions
    (
        Player& player,
        std::list<Crate>& crates
    ) 
    {
        bool wasOnGround = player.isOnGround();
        bool standingOnCrate = false;
        for (auto& crate : crates) 
        {
            if (!CheckCollision(player, crate)) continue;    
            const Vector playerPos = player.getPosition();
            const Vector playerSize = player.getSize();
            const Vector cratePos = crate.getPosition();
            const Vector crateSize = crate.getSize();
            const float playerBottom = 
                playerPos.y + playerSize.y;
            const float crateTop = cratePos.y;
            const float overlapVertical = 
                playerBottom - crateTop;
            const float playerRight = 
                playerPos.x + playerSize.x;
            const float crateLeft = cratePos.x;
            const float playerLeft = playerPos.x;
            const float crateRight = 
                cratePos.x + crateSize.x;
            const float overlapHorizontal = 
            std::min
            (
                playerRight - crateLeft, crateRight - playerLeft
            );  
            if (overlapVertical < overlapHorizontal) 
            {
                handleVerticalCrateCollision
                (
                    player, 
                    cratePos, 
                    playerPos, 
                    playerSize, 
                    standingOnCrate, 
                    wasOnGround
                );
            } 
            else 
            {
                handleHorizontalCrateCollision
                (
                    player, 
                    crate, 
                    crateLeft, 
                    crateRight, 
                    playerPos, 
                    playerSize
                );
            }
        }
        player.setOnGround(wasOnGround || standingOnCrate);
    }
    
    void PhysicsEngine::handleVerticalCrateCollision
    (
        Player& player,
        const Vector& cratePos,
        const Vector& playerPos,
        const Vector& playerSize,
        bool& standingOnCrate,
        bool& wasOnGround
    ) 
    {
        if (player.getVelocity().y > 0) 
        {
            player.setPosition(Vector(playerPos.x, cratePos.y - playerSize.y));
            player.setVelocity(Vector(player.getVelocity().x, 0));
            standingOnCrate = true;
            wasOnGround = true;
        }
    }
    
    void PhysicsEngine::handleHorizontalCrateCollision
    (
        Player& player,
        Crate& crate,
        float crateLeft,
        float crateRight,
        const Vector& playerPos,
        const Vector& playerSize
    ) 
    {
        const Vector pushForce(player.getVelocity().x * 1.2f, 0.0f);
        crate.applyForce(pushForce);
        if (player.getVelocity().x > 0) 
        {
            player.setPosition(Vector(crateLeft - playerSize.x, playerPos.y));
        } 
        else if (player.getVelocity().x < 0) 
        {
            player.setPosition(Vector(crateRight, playerPos.y));
        }
    }
} // namespace BRTC