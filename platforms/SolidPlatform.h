#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include <SDL2/SDL.h>
#include "../scenario/Scenario.h"

class SolidPlatform : public Scenario
{
public:
    SolidPlatform(float x, float y, float width, float height);
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
};

#endif // SOLIDPLATFORM_H
