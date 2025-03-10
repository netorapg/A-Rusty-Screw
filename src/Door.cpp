#include "Door.h"

Door::Door(float x, float y, float width, float height, const std::string& levelToLoad) : Scenario(x, y, width, height), mLevelToLoad(levelToLoad) {}

bool Door::isPlayerColliding(const Entity& player) const {
    return (player.getPosX() < getX() + getWidth() &&
    player.getPosX() + player.getWidth() > getX() &&
    player.getPosY() < getY() + getHeight() &&
    player.getPosY() + player.getHeight() > getY());
}

std::string Door::getLevelToLoad() const {
    return mLevelToLoad;
}

void Door::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    SDL_Rect doorRect = {
        static_cast<int>(getX() - cameraX),
        static_cast<int>(getY() - cameraY),
        static_cast<int>(getWidth()),
        static_cast<int>(getHeight())
    };

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &doorRect);
}