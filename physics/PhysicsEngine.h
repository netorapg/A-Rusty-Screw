#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <list>
#include "../entity/Entity.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"

class PhysicsEngine {
    public: 
    static void HandleCollisions(Entity& entity,
                                  const std::list<Platform>& platforms,
                                  const std::list<SolidPlatform>& solidPlatforms,
                                  const std::list<Wall>& walls,
                                  std::list<Crate>& crates);
};

#endif // PHYSICS_ENGINE_H