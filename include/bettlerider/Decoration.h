#ifndef DECORATION_H
#define DECORATION_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Animation.h"

namespace BRTC {
class Decoration : public StaticObject {
private:
    Animation mAnimation;
public:
    Decoration(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath);
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
}; //
 
}

#endif // DECORATION_H