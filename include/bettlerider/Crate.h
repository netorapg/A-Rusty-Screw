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
    Crate(Vector position, SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
    void update(float deltaTime) override; // Assinatura modificada
    
    // Novos métodos
    void applyForce(Vector force);
    //bool checkCollision(float x, float y, float w, float h) const;
    
    // Método vazio para eventos (obrigatório por DynamicObject)
    void handleEvent(SDL_Event& e) override {}
    
private:
    const float mGravity = 0.3f;
    const float mFriction = 0.85f;
};

} // namespace BRTC
#endif // CRATE_H