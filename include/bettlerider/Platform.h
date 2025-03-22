#ifndef PLATFORM_H
#define PLATFORM_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>

namespace BRTC {

class Platform : public StaticObject {
private:
    Sprite mSprite; // Sprite da plataforma

public:
    Platform(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath);
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
};

} // namespace BRTC

#endif // PLATFORM_H