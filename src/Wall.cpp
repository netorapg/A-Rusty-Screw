#include "../include/bettlerider/Wall.h"

namespace BRTC
{

Wall::Wall(const Vector position, const Vector size, SDL_Renderer* renderer, const std::string& texturePath)
    : StaticObject(position, size),
      mSprite(renderer, texturePath)
{
    Animation wallAnim;
    wallAnim.addFrame({
        { 82, 49, static_cast<int>(size.x), static_cast<int>(size.y) },
        1.0f,
        { 0, 0 }
    });
    wallAnim.setLoop(true);

    mSprite.addAnimation("default", std::move(wallAnim));
    mSprite.play("default");
}

void Wall::render(SDL_Renderer* renderer, Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    mSprite.draw(renderer, screenPosition.x, screenPosition.y);
}

}