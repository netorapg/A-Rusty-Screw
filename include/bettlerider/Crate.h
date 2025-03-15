#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include "Object.h" // Substitua Scenario.h
#include <list>
#include "SolidPlatform.h"
#include "Platform.h"
#include "Wall.h"
#include "../../my-lib-master/include/my-lib/math-vector.h"

namespace BRTC
{

class Crate : public DynamicObject { // Herda de DynamicObject
public:
    Crate(float x, float y, float width, float height);
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
    void update() override; // Assinatura modificada
   // bool isVisible(float cameraX, float cameraY, float screenWidth, float screenHeight) override; // Tipos ajustados
   /*bool Crate::isVisible(float cameraX, float cameraY, float screenWidth, float screenHeight) {
    return DynamicObject::isVisible(cameraX, cameraY, screenWidth, screenHeight);
}*/
    
    // Novos métodos
    void applyForce(float fx, float fy);
    bool checkCollision(float x, float y, float w, float h) const;
    
    // Método vazio para eventos (obrigatório por DynamicObject)
    void handleEvent(SDL_Event& e) override {}
    
private:
    const float mGravity = 0.3f;
    const float mFriction = 0.85f;
    
    // Remova mVel (usaremos mVelX e mVelY de DynamicObject)
    void handlePlatformCollisions(const std::list<SolidPlatform>& SolidPlatforms,
                                  const std::list<Platform>& Platforms);
    void handleWallCollisions(const std::list<Wall>& Walls);
};

} // namespace BRTC
#endif // CRATE_H