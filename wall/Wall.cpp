#include "Wall.h"

Wall::Wall(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Wall::render(SDL_Renderer *renderer) {
    SDL_Rect fillRect = {static_cast<int>(mPosX), static_cast<int>(mPosY), static_cast<int>(mWidth), static_cast<int>(mHeight)};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // White color for walls
    SDL_RenderFillRect(renderer, &fillRect);
}
