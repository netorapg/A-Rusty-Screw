#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "../Object.h"

class Platform : public Object
{
public:
    Platform(float x, float y, float width, float height);

    void render(SDL_Renderer *renderer);
};

#endif // PLATFORM_H