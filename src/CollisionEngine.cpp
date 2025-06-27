#include "../include/arscrew/CollisionEngine.h"

namespace ARSCREW
{
    bool CollisionEngine::CheckCollision(const Object& staticObj, const Object& dynamicObj)
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

    bool CollisionEngine::CheckRampCollision(const Ramp& ramp, const Vector& point)
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
            return (relative.x <= rampSize.x) && (relative.y >= 0) &&
                    ((relative.y / rampSize.y) <= (relative.x / rampSize.x));
            case RampType::TOP_LEFT:
            return (relative.x >= 0) && (relative.y <= rampSize.y) &&
                    ((relative.y / rampSize.y) >= (1 - relative.x / rampSize.x));
            case RampType::TOP_RIGHT:
            return (relative.x <= rampSize.x) && (relative.y <= rampSize.y) &&
                    ((relative.y / rampSize.y) >= (relative.x / rampSize.x));
            default:
            return false;
        }
    }
    
    void CollisionEngine::HandleCollisions
    (
        DynamicObject &dynamicObject,
        const std::list<Platform> &platforms,
        const std::list<SolidPlatform> &solidPlatforms,
        const std::list<Ramp> &ramps
    )
    {
        Vector position = dynamicObject.getPosition();
        const Vector size = dynamicObject.getSize();
        Vector velocity = dynamicObject.getVelocity();    
        
        bool hasPlatformCollision = false;
        
        // Só processar colisões com plataformas normais se NÃO estiver passando através
        if (!dynamicObject.isPassingThroughPlatform()) 
        {
            hasPlatformCollision = handlePlatformCollisions
            (
                dynamicObject, 
                platforms, 
                position, 
                velocity
            );
        }
        
        // Sempre processar colisões com plataformas sólidas
        hasPlatformCollision |= handleSolidPlatformCollisions
        (
            dynamicObject, 
            solidPlatforms, 
            position, 
            velocity
        );
        
        bool hasRampCollision = handleRampCollisions(dynamicObject, ramps, position, velocity);
        updateGroundState(dynamicObject, hasPlatformCollision || hasRampCollision);
        dynamicObject.setPosition(position);
        dynamicObject.setVelocity(velocity);
    }


    bool CollisionEngine::handleSolidPlatformCollisions(
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
            
            // Adiciona uma pequena tolerância para evitar travamentos
            const float tolerance = 2.0f;
            
            // Prioriza colisões verticais quando a diferença é pequena
            bool isVerticalCollision = (overlapTop <= overlapLeft + tolerance && 
                                  overlapTop <= overlapRight + tolerance) ||
                                 (overlapBottom <= overlapLeft + tolerance && 
                                  overlapBottom <= overlapRight + tolerance);
            
            // Colisão vertical (topo da plataforma) - priorizada
            if (isVerticalCollision && 
                overlapTop < overlapBottom && 
                velocity.y >= 0 && 
                overlapTop > 0)
            {
                position.y = platformPos.y - size.y;
                velocity.y = 0;
                dynamicObject.setOnGround(true);
                dynamicObject.setFalling(false);
                continue;
            } 
            // Colisão vertical (parte inferior da plataforma)
            else if (isVerticalCollision &&
                     overlapBottom < overlapTop && 
                     velocity.y <= 0 && 
                     overlapBottom > 0)
            {
                position.y = platformPos.y + platformSize.y;
                velocity.y = 0;
                dynamicObject.setFalling(true);
                continue;
            }
            // Colisão horizontal (lado esquerdo da plataforma)
            else if (overlapLeft < overlapTop - tolerance && 
                     overlapLeft < overlapBottom - tolerance && 
                     overlapLeft < overlapRight)
            {
                // Adicionar uma pequena margem de segurança para evitar penetração visual
                position.x = platformPos.x - size.x - 1.0f;
                velocity.x = 0;
                // Informar ao jogador que houve colisão lateral (se for um Player)
                Player* player = dynamic_cast<Player*>(&dynamicObject);
                if (player) {
                    player->setHadLateralCollision(true);
                    // Ativar buffer para evitar movimento por alguns frames
                    player->setLateralCollisionBuffer(0.1f); // 100ms de buffer
                }
                // Permitir wall jump em SolidPlatforms
                dynamicObject.setIsCollidingWithWall(true);
            }
            // Colisão horizontal (lado direito da plataforma)
            else if (overlapRight < overlapTop - tolerance && 
                     overlapRight < overlapBottom - tolerance && 
                     overlapRight < overlapLeft)
            {
                // Adicionar uma pequena margem de segurança para evitar penetração visual
                position.x = platformPos.x + platformSize.x + 1.0f;
                velocity.x = 0;
                // Informar ao jogador que houve colisão lateral (se for um Player)
                Player* player = dynamic_cast<Player*>(&dynamicObject);
                if (player) {
                    player->setHadLateralCollision(true);
                    // Ativar buffer para evitar movimento por alguns frames
                    player->setLateralCollisionBuffer(0.1f); // 100ms de buffer
                }
                // Permitir wall jump em SolidPlatforms - mesmo comportamento do lado esquerdo
                dynamicObject.setIsCollidingWithWall(true);
            }
        }
        
        return collisionOccurred;
    }
    
    bool CollisionEngine::handlePlatformCollisions
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

bool CollisionEngine::handleRampCollisions(
    DynamicObject& dynamicObject,
    const std::list<Ramp>& ramps,
    Vector& position,
    Vector& velocity)
{
    bool collisionOccurred = false;
    const Vector size = dynamicObject.getSize();
    const float feetOffset = 2.0f;

    for (const auto& ramp : ramps) {
        if (!CheckCollision(dynamicObject, ramp)) continue;

        const Vector rampPos = ramp.getPosition();
        const Vector rampSize = ramp.getSize();

        Vector checkPoints[] = {
            Vector(position.x + size.x * 0.25f, position.y + size.y - feetOffset),
            Vector(position.x + size.x * 0.5f, position.y + size.y - feetOffset),
            Vector(position.x + size.x * 0.75f, position.y + size.y - feetOffset)
        };

        for (const auto& point : checkPoints) {
            if (point.x < rampPos.x || point.x > rampPos.x + rampSize.x) continue;

            float surfaceY = ramp.getSurfaceY(point.x);
            if (point.y < surfaceY) continue;

            collisionOccurred = true;
            position.y = surfaceY - size.y + feetOffset;
            dynamicObject.setOnGround(true);

            // Calcula a direção tangente à rampa (perpendicular à normal)
            Vector tangent;
            switch(ramp.getType()) {
                case RampType::BOTTOM_LEFT:
                    tangent = Vector(rampSize.x, rampSize.y);
                    break;
                case RampType::BOTTOM_RIGHT:
                    tangent = Vector(rampSize.x, -rampSize.y);
                    break;
                case RampType::TOP_LEFT:
                    tangent = Vector(rampSize.x, -rampSize.y);
                    break;
                case RampType::TOP_RIGHT:
                    tangent = Vector(rampSize.x, rampSize.y);
                    break;
            }

            // Normaliza a tangente
            tangent = CollisionEngine::normalize(tangent);

            // Mantém a magnitude original da velocidade
            float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            
            // Aplica a velocidade na direção da tangente
            velocity.x = tangent.x * speed;
            velocity.y = tangent.y * speed;

            // Se estiver parado, zera a velocidade vertical
            if (std::abs(velocity.x) < 0.1f) {
                velocity.y = 0;
            }

            break;
        }
    }

    return collisionOccurred;
}


    void CollisionEngine::updateGroundState
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

    bool CollisionEngine::HandlePlayerCollisions
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

    bool CollisionEngine::checkDoorCollisions
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
    
    void CollisionEngine::handleCrateCollisions
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
    
    void CollisionEngine::handleVerticalCrateCollision
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
    
    void CollisionEngine::handleHorizontalCrateCollision
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