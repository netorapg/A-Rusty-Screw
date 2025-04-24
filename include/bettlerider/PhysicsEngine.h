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
#include "Ramp.h"
#include "Wall.h"

namespace BRTC
{
  class PhysicsEngine
  {
    public:
    static  bool CheckCollision(const Object& staticObj, const Object& dynamicObj);
    static  bool CheckRampCollision(const Ramp& ramp, const Vector& point);
    static  void HandleCollisions
      (
        DynamicObject& dynamicObject,
        const std::list<Wall>& walls,
        const std::list<Platform>& platforms,
        const std::list<SolidPlatform>& solidPlatforms,
        const std::list<Ramp>& ramps
      );

    static  bool HandlePlayerCollisions
      ( 
        Player  &player,
        Vector &spawnPosition,
        const std::list<Door> &doors,
        std::list<Crate> &crates,
        std::string &levelToLoad
      );

      private:

      static bool handleWallCollisions
      (
        DynamicObject& dynamicObject, 
        const std::list<Wall>& walls,                    
        Vector& position, 
        Vector& velocity
      );
      
      static bool handleSolidPlatformCollisions
      (
        DynamicObject& dynamicObject,                                
        const std::list<SolidPlatform>& solidPlatforms,
        Vector& position, 
        Vector& velocity
      );

      static bool handlePlatformCollisions
      (
        DynamicObject& dynamicObject, 
        const std::list<Platform>& platforms,
        Vector& position, 
        Vector& velocity
      );

      static bool handleRampCollisions
      (
        DynamicObject& dynamicObject,
        const std::list<Ramp>& ramps,
        Vector& position,
        Vector& velocity
      );

      static void updateGroundState
      (
        DynamicObject& dynamicObject, 
        bool hasPlatformCollision
      );
      
      static bool checkDoorCollisions
      (
        const Player& player, 
        const std::list<Door>& doors,
        Vector& spawnPosition, 
        std::string& levelToLoad
      );

      static void handleCrateCollisions
      (
        Player& player, 
        std::list<Crate>& crates
      );

      static void handleVerticalCrateCollision
      (
        Player& player, 
        const Vector& cratePos,
        const Vector& playerPos, 
        const Vector& playerSize,
        bool& standingOnCrate, 
        bool& wasOnGround
      );

      static void handleHorizontalCrateCollision
      (
        Player& player, 
        Crate& crate,
        float crateLeft, 
        float crateRight,
        const Vector& playerPos, 
        const Vector& playerSize
      );
  };
}
#endif
