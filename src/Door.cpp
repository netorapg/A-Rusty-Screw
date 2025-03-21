#include "../include/bettlerider/Door.h"
#include "../include/bettlerider/Animation.h"

namespace BRTC
{

Door::Door(const Vector position, const Vector size, const std::string& levelToLoad, SDL_Renderer* renderer, const std::string& texturePath)
    : StaticObject(position, size),
      mSprite(renderer, texturePath),
      mLevelToLoad(levelToLoad)
{
    Animation doorAnim;
    doorAnim.addFrame({
        { 160, 79, static_cast<int>(size.x), static_cast<int>(size.y) },
        1.0f,
        { 0, 0 }
    });
    doorAnim.setLoop(true);

    mSprite.addAnimation("default", std::move(doorAnim));
    mSprite.play("default");
}

std::string Door::getLevelToLoad() const {
    return mLevelToLoad;
}

void Door::render(SDL_Renderer* renderer, Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    mSprite.draw(renderer, screenPosition.x, screenPosition.y);

}
}