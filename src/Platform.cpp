#include "../include/bettlerider/Platform.h"
#include "../include/bettlerider/Animation.h"

namespace BRTC {

Platform::Platform(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath)
    : StaticObject(position, size)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, texturePath.c_str());
    if (!texture) {
        SDL_Log("Failed to load texture: %s", SDL_GetError());
        return;
    }
    SpritePtr platformSprite = std::make_shared<Sprite>(texture, SDL_Rect{127, 36, static_cast<int>(size.x), static_cast<int>(size.y)});

    mAnimation.addFrame({platformSprite, 0.1f, {0, 0}});
    mAnimation.setLoop(false);
}

void Platform::render(SDL_Renderer* renderer, Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    SpritePtr currentSprite = mAnimation.getCurrentSprite();
    if (currentSprite) {
        currentSprite->draw(renderer, screenPosition.x, screenPosition.y);
    }
}

} // namespace BRTC