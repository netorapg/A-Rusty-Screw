#include "Crate.h"

Crate::Crate(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Crate::render(SDL_Renderer *renderer) {
    SDL_Rect fillRect = {static_cast<int>(mPosX), static_cast<int>(mPosY), static_cast<int>(mWidth), static_cast<int>(mHeight)};
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_RenderFillRect(renderer, &fillRect);
}