#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "../Scenario.h"

class Platform : public Scenario
{
public:
    Platform(float x, float y, float width, float height);
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
};

#endif // PLATFORM_H
