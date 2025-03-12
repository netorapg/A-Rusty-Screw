#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "../../entity/Entity.h"
#include "Crate.h"
#include "Door.h"
#include "Platform.h"
#include "SolidPlatform.h"
#include "Wall.h"

#include <list>

namespace BRTC
{

class PhysicsEngine
{
public:
  static void HandleCollisions( Entity                         &entity,
                                const std::list<Platform>      &platforms,
                                const std::list<SolidPlatform> &solidPlatforms,
                                const std::list<Wall>          &walls,
                                std::list<Crate>               &crates,
                                const std::list<Door>          &doors,
                                std::string                    &levelToLoad );
};

}   // namespace BRTC
#endif   // PHYSICS_ENGINE_H
