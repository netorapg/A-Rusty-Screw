#ifndef COLLISION_ENGINE_H
#define COLLISION_ENGINE_H

#include <list>
#include <iostream>
#include "Crate.h"
#include "Door.h"
#include "Object.h"
#include "Platform.h"
#include "Player.h"
#include "SolidPlatform.h"

namespace ARSCREW
{
  class CollisionEngine
  {
    public:
    static  bool CheckCollision(const Object& staticObj, const Object& dynamicObj);

   static Vector normalize(const Vector& v) {
        float length = std::sqrt(v.x * v.x + v.y * v.y);
        if (length > 0.0f) {
            return Vector(v.x / length, v.y / length);
        }
        return Vector(0, 0);
    }

    static  void HandleCollisions
      (
        DynamicObject& dynamicObject,
        const std::list<Platform>& platforms,
        const std::list<SolidPlatform>& solidPlatforms
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
