#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>

namespace BRTC {

class SolidPlatform : public StaticObject {
private:
    Sprite mSprite; // Sprite da plataforma sólida

public:
    SolidPlatform(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath);
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
};

} // namespace BRTC

#endif // SOLIDPLATFORM_H