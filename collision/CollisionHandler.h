#ifndef COLLISION_HANDLER_H
#define COLLISION_HANDLER_H

#include <list>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"
#include "../my-lib-master/include/my-lib/math-vector.h"

class CollisionHandler {
public:
    static bool checkCollision(float x1, float y1, float w1, float h1, 
                               float x2, float y2, float w2, float h2);
                               
    static void handleWallCollision(float& posX, float& velX, float width, const std::list<Wall>& walls);
    
    static void handleCrateCollision(float& posX, float& posY, float& velX, float& velY, 
                                     bool& aboveCrate, bool& onGround, bool& falling,
                                     std::list<Crate>& crates);

    static void handlePlatformCollision(float& posY, float& velY, bool& onGround, bool& falling, 
                                        const std::list<Platform>& platforms, 
                                        const std::list<SolidPlatform>& solidPlatforms, 
                                        bool passingThroughPlatform, float playerWidth);
};

#endif // COLLISION_HANDLER_H
