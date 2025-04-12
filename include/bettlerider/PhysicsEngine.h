#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <list>
#include <iostream>
#include "Crate.h"
#include "Door.h"
#include "Object.h"
#include "Platform.h"
#include "Player.h"
#include "SolidPlatform.h"
#include "Wall.h"

namespace BRTC
{
  class PhysicsEngine
  {
    public:
      static void HandleCollisions
      (
        DynamicObject &DynamicObject,
        const std::list<Wall> &walls,
        const std::list<Platform> &platforms,
        const std::list<SolidPlatform> &solidPlatforms 
      );
      static bool HandlePlayerCollisions
      ( 
        Player  &player,
        Vector &spawnPosition,
        const std::list<Door> &doors,
        std::list<Crate> &crates,
        std::string &levelToLoad
      );
    private:
      static bool CheckCollision( const Object& obj1, const Object& obj2 );
  };
}
#endif
