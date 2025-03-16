#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "Crate.h"
#include "Door.h"
#include "Object.h"
#include "Platform.h"
#include "Player.h"
#include "SolidPlatform.h"
#include "Wall.h"

#include <list>

namespace BRTC
{

class PhysicsEngine
{
public:
  static void HandleCollisions(
    DynamicObject                  &DynamicObject,
    const std::list<Wall>          &walls,
    const std::list<Platform>      &platforms,
    const std::list<SolidPlatform> &solidPlatforms );
  static void HandlePlayerCollisions( Player                &player,
                                      std::list<Crate>      &crates,
                                      const std::list<Door> &doors,
                                      std::string           &levelToLoad );

private:
  static bool CheckCollision( const Object& obj1, const Object& obj2 );
};   // class PhysicsEngine

}   // namespace BRTC
#endif   // PHYSICS_ENGINE_H
