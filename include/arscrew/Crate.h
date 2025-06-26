#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include <list>
#include "Object.h"
#include "Globals.h"
#include "SolidPlatform.h"
#include "Platform.h"

namespace ARSCREW
{

    class Crate : public DynamicObject 
    {
        public:
            Crate(Vector position, SDL_Renderer* renderer);
            void render(SDL_Renderer* renderer, Vector cameraPosition) override;
            void update(float deltaTime) override;
            void applyForce(Vector force);
    };
}
#endif