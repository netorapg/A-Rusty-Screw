#include "../include/bettlerider/PhysicsEngine.h"
#include <iostream>

namespace BRTC
{
    bool PhysicsEngine::CheckCollision(const Object& obj1, const Object& obj2) 
    {
        const Vector pos1 = obj1.getPosition();
        const Vector size1 = obj1.getSize();
        Vector pos2 = obj2.getPosition();
        Vector size2 = obj2.getSize();
        
        return (pos1.x < pos2.x + size2.x &&
                pos1.x + size1.x > pos2.x &&
                pos1.y < pos2.y + size2.y &&
                pos1.y + size1.y > pos2.y);
    }

    void PhysicsEngine::HandleCollisions(
        DynamicObject& dynamicObject,
        const std::list<Wall>& walls,
        const std::list<Platform>& platforms,
        const std::list<SolidPlatform>& solidPlatforms
    ) {
        Vector position = dynamicObject.getPosition();
        const Vector size = dynamicObject.getSize();
        Vector velocity = dynamicObject.getVelocity();

        // Colisão com paredes
        for (const auto& wall : walls) {
            if (CheckCollision(dynamicObject, wall)) {
                const Vector wallPos = wall.getPosition();
                const Vector wallSize = wall.getSize();
                
                // Resolve colisão horizontal
                if (velocity.x > 0) {
                    position.x = wallPos.x - size.x;
                } else if (velocity.x < 0) {
                    position.x = wallPos.x + wallSize.x;
                }
                velocity.x = 0;
            }
        }

        // Colisão com plataformas sólidas
        for (const auto& platform : solidPlatforms) {
            if (CheckCollision(dynamicObject, platform)) {
                const Vector platformPos = platform.getPosition();
                const Vector platformSize = platform.getSize();
                
                // Resolve colisão vertical
                if (velocity.y > 0) {
                    position.y = platformPos.y - size.y;
                    velocity.y = 0;
                    dynamicObject.setOnGround(true);
                    std::cout << "isOnGround: " << dynamicObject.isOnGround() << std::endl;
                } else if (velocity.y < 0) {
                    position.y = platformPos.y + platformSize.y;
                    velocity.y = 0;
                }
            }
        }

        // Colisão com plataformas normais
        if (!dynamicObject.isPassingThroughPlatform()) {
            for (const auto& platform : platforms) {
                if (CheckCollision(dynamicObject, platform)) {
                    const Vector platformPos = platform.getPosition();
                    const Vector platformSize = platform.getSize();
                    
                    if (velocity.y > 0) {
                        position.y = platformPos.y - size.y;
                        velocity.y = 0;
                        dynamicObject.setOnGround(true);
                        std::cout << "isOnGround: " << dynamicObject.isOnGround() << std::endl;
                    } else if (velocity.y < 0) {
                        position.y = platformPos.y - platformSize.y;
                        velocity.y = 0;

                    }
                }
            }
        }

        // Aplica mudanças
        dynamicObject.setPosition(position);
        dynamicObject.setVelocity(velocity);
    }

    bool PhysicsEngine::HandlePlayerCollisions(
        Player& player,
        std::list<Crate>& crates,
        const std::list<Door>& doors,
        std::string& levelToLoad,
        Vector& spawnPosition
    ) {


        for (auto& door : doors) {
            if (CheckCollision(player, door)) {
                levelToLoad = door.getLevelToLoad();
                if (door.hasValidSpawn()) {
                    spawnPosition = door.getSpawnPosition();
                } else {
                    spawnPosition = Vector(-1, -1);
                }
                return true;
            }
        }

        bool wasOnGround = player.isOnGround();
        bool standingOnCrate = false;

        for (auto& crate : crates) {
            if (CheckCollision(player, crate)) {
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

                // Colisão vertical (jogador em cima/embaixo)
                if (overlapVertical < overlapHorizontal) {
                    if (player.getVelocity().y > 0) { // Caindo
                        player.setPosition(Vector(playerPos.x, crateTop - playerSize.y));
                        player.setVelocity(Vector(player.getVelocity().x, 0));
                        standingOnCrate = true;
                        wasOnGround = true;
                    }
                }
                // Colisão horizontal (empurrar)
                else {
                    // Empurra a caixa com força proporcional à velocidade do jogador
                    const Vector pushForce(player.getVelocity().x * 1.2f, 0.0f);
                    crate.applyForce(pushForce);
                    
                    // Ajusta posição do jogador para evitar sobreposição
                    if (player.getVelocity().x > 0) { // Movendo para direita
                        player.setPosition(Vector(crateLeft - playerSize.x, playerPos.y));
                    } else if (player.getVelocity().x < 0) { // Movendo para esquerda
                        player.setPosition(Vector(crateRight, playerPos.y));
                    }
                }
            }
        }

        player.setOnGround(wasOnGround || standingOnCrate);
        return false;
       
    }
} // namespace BRTC