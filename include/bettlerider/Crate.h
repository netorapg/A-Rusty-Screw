#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include "../scenario/Scenario.h"
#include <list>
#include "SolidPlatform.h"
#include "Platform.h"
#include "Wall.h"
#include "math-vector.h"

class Crate : public Scenario {
public:
    Crate(float x, float y, float width, float height);
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
    void update(const std::list<SolidPlatform>& SolidPlatforms, 
               const std::list<Wall>& Walls,
               const std::list<Platform>& Platforms);
    bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) override;
    
    // Novos métodos
    void applyForce(float fx, float fy);
    Mylib::Math::Vector2f getVelocity() const { return mVel; }
    bool checkCollision(float x, float y, float w, float h) const;

private:
    Mylib::Math::Vector2f mVel;
    const float mGravity = 0.3f;
    const float mFriction = 0.85f;

    void handlePlatformCollisions(const std::list<SolidPlatform>& SolidPlatforms,
                                  const std::list<Platform>& Platforms);
    void handleWallCollisions(const std::list<Wall>& Walls);
};
#endif // CRATE_H