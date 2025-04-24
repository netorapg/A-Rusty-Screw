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

    bool PhysicsEngine::CheckRampCollision(const Ramp& ramp, const Vector& point)
    {
        const Vector rampPos = ramp.getPosition();
        const Vector rampSize = ramp.getSize();
        Vector relative;
        relative.x = point.x - rampPos.x;
        relative.y = point.y - rampPos.y;

        switch(ramp.getType()) {
            case RampType::BOTTOM_LEFT:
            return (relative.x >= 0) && (relative.y >= 0) &&
                    ((relative.y / rampSize.y) <= (1 - relative.x / rampSize.x));
            case RampType::BOTTOM_RIGHT:
                
        }
    }
    
    void PhysicsEngine::HandleCollisions
    (
        DynamicObject &dynamicObject,
        const std::list<Wall> &walls,
        const std::list<Platform> &platforms,
        const std::list<SolidPlatform> &solidPlatforms,
        const std::list<Ramp> &ramps
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
        bool hasRampCollision = handleRampCollisions(dynamicObject, ramps, position, velocity);
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
        updateGroundState(dynamicObject, hasPlatformCollision || hasRampCollision);
        dynamicObject.setPosition(position);
        dynamicObject.setVelocity(velocity);
    }

    bool PhysicsEngine::handleWallCollisions(
        DynamicObject &dynamicObject,
        const std::list<Wall> &walls,
        Vector& position,
        Vector& velocity
    ) {
        bool collisionOccurred = false;
        const Vector size = dynamicObject.getSize();
    
        for (const auto& wall : walls) {
            if (!CheckCollision(dynamicObject, wall)) {
                continue;
            }
    
            collisionOccurred = true;
            const Vector wallPos = wall.getPosition();
            const Vector wallSize = wall.getSize();
    
            // Calcula a sobreposição em X e Y para determinar o lado da colisão
            float overlapLeft = (position.x + size.x) - wallPos.x;
            float overlapRight = (wallPos.x + wallSize.x) - position.x;
            float overlapTop = (position.y + size.y) - wallPos.y;
            float overlapBottom = (wallPos.y + wallSize.y) - position.y;
    
            // Determina qual é a menor sobreposição (lado da colisão)
            bool fromLeft = overlapLeft < overlapRight;
            bool fromTop = overlapTop < overlapBottom;
    
            // Se a colisão for mais horizontal (paredes laterais)
            if (overlapLeft < overlapTop && overlapLeft < overlapBottom) {
                // Colisão pela esquerda
                position.x = wallPos.x - size.x;
                velocity.x = 0;
                std::cout << "Collision on LEFT side" << std::endl;
            }
            else if (overlapRight < overlapTop && overlapRight < overlapBottom) {
                // Colisão pela direita
                position.x = wallPos.x + wallSize.x;
                velocity.x = 0;
                std::cout << "Collision on RIGHT side" << std::endl;
            }
            else if (overlapTop < overlapLeft && overlapTop < overlapRight) {
                // Colisão por cima (opcional, se paredes tiverem topo)
                position.y = wallPos.y - size.y;
                velocity.y = 0;
                std::cout << "Collision on TOP side" << std::endl;
            }
            else if (overlapBottom < overlapLeft && overlapBottom < overlapRight) {
                // Colisão por baixo (opcional, se paredes tiverem base)
                position.y = wallPos.y + wallSize.y;
                velocity.y = 0;
                std::cout << "Collision on BOTTOM side" << std::endl;
            }
    
            dynamicObject.setIsCollidingWithWall(true);
        }
    
        return collisionOccurred;
    }

    bool PhysicsEngine::handleSolidPlatformCollisions(
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
            
       
            float overlapTop = (position.y + size.y) - platformPos.y;
            float overlapBottom = (platformPos.y + platformSize.y) - position.y;
            float overlapLeft = (position.x + size.x) - platformPos.x;
            float overlapRight = (platformPos.x + platformSize.x) - position.x;
            
            
            if (overlapTop < overlapBottom && 
                overlapTop < overlapLeft && 
                overlapTop < overlapRight && 
                velocity.y >= 0) // Colisão pelo topo
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);
                dynamicObject.setFalling(false);

                continue;
            } 
            else if (overlapBottom < overlapTop && 
                     overlapBottom < overlapLeft && 
                     overlapBottom < overlapRight && 
                     velocity.y <= 0)
            {
                position.y = platformPos.y + platformSize.y;
                velocity.y = 0;
                dynamicObject.setFalling(true);
            }
  
            /*else if (overlapLeft < overlapTop && 
                     overlapLeft < overlapBottom && 
                     overlapLeft < overlapRight &&
                     position.y + size.y > platformSize.y  + 3.0f &&
                     position.y < platformPos.y + platformSize.y - 3.0f &&
                    std::abs(velocity.x) > 1.1f)
            {
                position.x = platformPos.x - size.x;
                velocity.x = 0;
            }
            else if (overlapRight < overlapTop && 
                     overlapRight < overlapBottom && 
                     overlapRight < overlapLeft &&
                     position.y + size.y > platformSize.y  + 3.0f &&
                     position.y < platformPos.y + platformSize.y - 3.0f &&
                    std::abs(velocity.x) > 1.1f)
            {
                position.x = platformPos.x + platformSize.x + 0.1f;
                velocity.x = 0;
            }*/
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
            
        
            float overlapTop = (position.y + size.y) - platformPos.y;
            float overlapBottom = (platformPos.y + platformSize.y) - position.y;
            float overlapLeft = (position.x + size.x) - platformPos.x;
            float overlapRight = (platformPos.x + platformSize.x) - position.x; 
            

            if (overlapTop < overlapBottom && 
                overlapTop < overlapLeft && 
                overlapTop < overlapRight && 
                velocity.y >= 0 && 
                overlapTop > -5.0f)
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);

                continue;
            }
        }
        return collisionOccurred;
    }

    bool PhysicsEngine::handleRampCollisions
    (
        DynamicObject& dynamicObject,
        const std::list<Ramp>& ramps,
        Vector& position,
        Vector& velocity
    )
    {
        bool collisionOcurred = false;
        const Vector size = dynamicObject.getSize();

        for (const auto& ramp: ramps)
        {
            if(!CheckCollision(dynamicObject, ramp)) continue;

            collisionOcurred = true;
            const Vector rampPos = ramp.getPosition();
            const Vector rampSize = ramp.getSize();

            float overlapTop = (position.y + size.y) - rampPos.y;
            float overlapBottom = (rampPos.y + rampSize.y) - position.y;
            float overlapLeft = (position.x + size.x) - rampPos.x;
            float overlapRight = (rampPos.x + rampSize.x ) - position.x;

            if (overlapTop < overlapBottom &&
                overlapTop < overlapLeft &&
                overlapTop < overlapRight &&
                velocity.y >= 0 &&
                overlapTop < -5.0f
            )
            {
                position.y = rampPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);

                continue;
            }
        }
        return collisionOcurred;
    }

    void PhysicsEngine::updateGroundState
    (
        DynamicObject& dynamicObject, 
        bool hasPlatformCollision
    ) 
    {
        if (!hasPlatformCollision && dynamicObject.isOnGround()) 
        {
           // dynamicObject.setIsCollidingWithWall(true);
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