#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "Crate.h"
#include "Door.h"
#include "Object.h"
#include "Platform.h"
#include "SolidPlatform.h"
#include "Wall.h"
#include "Player.h"

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
  static void HandlePlayerCollisions( Player         &player,
                                      std::list<Crate>      &crates,
                                      const std::list<Door> &doors,
                                      std::string           &levelToLoad );
};

}   // namespace BRTC
#endif   // PHYSICS_ENGINE_H
