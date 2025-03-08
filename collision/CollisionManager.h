// CollisionManager.h
#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "../crate/Crate.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"

#include <list>

class CollisionManager
{
public:
  static void handlePlatformCollision(
    float                    &posY,
    float                    &velY,
    bool                     &onGround,
    bool                     &falling,
    std::list<Platform>      &platforms,
    std::list<SolidPlatform> &solidPlatforms );

  static void handleWallCollision( float           &posX,
                                   float           &velX,
                                   std::list<Wall> &walls );

  static void handleCrateCollision( float            &posX,
                                    float            &posY,
                                    float            &velX,
                                    float            &velY,
                                    bool             &aboveCrate,
                                    bool             &onGround,
                                    bool             &falling,
                                    std::list<Crate> &crates );
};

#endif   // COLLISION_MANAGER_H
