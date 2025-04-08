#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Animation.h"

namespace BRTC {

class SolidPlatform : public StaticObject {
private:
    Animation mAnimation;

public:
    SolidPlatform(const Vector position, const Vector size, SDL_Texture* texture, int tileId);
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
};

} // namespace BRTC

#endif // SOLIDPLATFORM_H