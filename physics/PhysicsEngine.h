#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "../crate/Crate.h"
#include "../entity/Entity.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../door/Door.h"

#include <list>

class PhysicsEngine
{
public:
  static void HandleCollisions( Entity                         &entity,
                                const std::list<Platform>      &platforms,
                                const std::list<SolidPlatform> &solidPlatforms,
                                const std::list<Wall>          &walls,
                                std::list<Crate>         &crates,
                                const std::list<Door>          &doors,
                                std::string                    &levelToLoad
                              );
};

#endif   // PHYSICS_ENGINE_H
