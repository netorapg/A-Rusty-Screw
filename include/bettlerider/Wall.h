#ifndef WALL_H
#define WALL_H

#include <SDL2/SDL.h>
#include "Object.h"

namespace BRTC
{
    class Wall : public StaticObject
{
public:
    Wall(float x, float y, float width, float height) : StaticObject(x, y, width, height) {}
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
};
}
#endif // WALL_H
