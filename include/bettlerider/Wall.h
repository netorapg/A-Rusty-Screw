#ifndef WALL_H
#define WALL_H

#include <SDL2/SDL.h>
#include "Object.h"
#include "Sprite.h"

namespace BRTC
{
    class Wall : public StaticObject
{
private:
Sprite mSprite;
public:
Wall(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath);

void render(SDL_Renderer* renderer, Vector cameraPosition) override;
};
}
#endif // WALL_H
