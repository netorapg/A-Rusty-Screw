#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "Object.h"
namespace BRTC
{
class Platform : public StaticObject
{
public:
    Platform(float x, float y, float width, float height) : StaticObject(x, y, width, height) {}
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
};
}

#endif // PLATFORM_H
