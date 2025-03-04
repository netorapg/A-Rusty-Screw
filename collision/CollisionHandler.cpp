#include "CollisionHandler.h"

bool CollisionHandler::checkCollision(float x1, float y1, float w1, float h1, 
                                      float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

void CollisionHandler::handleWallCollision(float& posX, float& velX, float width, const std::list<Wall>& walls) {
    for (const auto& wall : walls) {
        if (checkCollision(posX, 0, width, 1, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight())) {
            if (velX > 0) {
                posX = wall.getX() - width;
            } else if (velX < 0) {
                posX = wall.getX() + wall.getWidth();
            }
            velX = 0;
        }
    }
}

void CollisionHandler::handleCrateCollision(float& posX, float& posY, float& velX, float& velY, 
                                            bool& aboveCrate, bool& onGround, bool& falling, 
                                            std::list<Crate>& crates) {
    for (auto &crate : crates) {
        if (checkCollision(posX, posY, 25, 26, crate.getX(), crate.getY(), crate.getWidth(), crate.getHeight())) {
            if (velX > 0 && !aboveCrate) {
                crate.setX(crate.getX() + velX);
                posX = crate.getX() - 25;
            } else if (velX < 0 && !aboveCrate) {
                crate.setX(crate.getX() + velX);
                posX = crate.getX() + crate.getWidth();
            } else if (velY > 0) {
                posY = crate.getY() - 26;
                aboveCrate = true;
                falling = false;
                onGround = true;
                velY = 0;
            }
        } else if (aboveCrate) {
            if (posY + 26 < crate.getY() || posY > crate.getY() + crate.getHeight()) {
                aboveCrate = false;
            }
        }
    }
}

void CollisionHandler::handlePlatformCollision(float& posY, float& velY, bool& onGround, bool& falling, 
                                               const std::list<Platform>& platforms, 
                                               const std::list<SolidPlatform>& solidPlatforms, 
                                               bool passingThroughPlatform, float playerWidth) {
    for (const auto& platform : solidPlatforms) {
        if (checkCollision(0, posY, playerWidth, 26, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight())) {
            if (velY > 0) {
                posY = platform.getY() - 26;
                velY = 0;
                onGround = true;
                falling = false;
            } else if (velY < 0) {
                posY = platform.getY() + platform.getHeight();
                velY = 0;
            }
        }
    }

    for (const auto &platform : platforms) {
        if (!passingThroughPlatform && 
            checkCollision(0, posY + playerWidth, 26, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()) && velY >= 0) {
            posY = platform.getY() - 26;
            falling = false;
            velY = 0;
            onGround = true;
            break;
        }
    }

    if (!onGround) {
        falling = true;
    }
}
