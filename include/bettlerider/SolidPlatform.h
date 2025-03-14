#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include <SDL2/SDL.h>
#include "Object.h"

namespace BRTC
{

class SolidPlatform : public StaticObject
{
public:
    SolidPlatform(float x, float y, float width, float height) : StaticObject(x, y, width, height) {}
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
};

}

#endif // SOLIDPLATFORM_H
