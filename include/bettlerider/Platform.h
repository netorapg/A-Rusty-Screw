#ifndef PLATFORM_H
#define PLATFORM_H

#include "Object.h"
#include <SDL2/SDL.h>
#include "Globals.h"

namespace BRTC {

class Platform : public StaticObject
{
public:
    Platform(const Vector position, const Vector size)
        : StaticObject(position, size) {}
    
    void render(SDL_Renderer* renderer,Vector cameraPosition) override;
};

} // namespace BRTC

#endif // PLATFORM_H