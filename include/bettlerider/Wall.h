#ifndef WALL_H
#define WALL_H

#include <SDL2/SDL.h>
#include "Object.h"

namespace BRTC
{
    class Wall : public StaticObject
{
public:
Wall(const Vector position, const Vector size)
: StaticObject(position, size) {}

void render(Vector cameraPosition) override;
};
}
#endif // WALL_H
