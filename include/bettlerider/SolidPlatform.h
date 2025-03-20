#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include <SDL2/SDL.h>
#include "Object.h"

namespace BRTC
{

class SolidPlatform : public StaticObject
{
public:
    SolidPlatform(const Vector position, const Vector size) : StaticObject(position, size) {}
    void render(SDL_Renderer* renderer, Vector cameraPosition) override; 
};

}

#endif // SOLIDPLATFORM_H
