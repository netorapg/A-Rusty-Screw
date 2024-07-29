#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Platform::render(SDL_Renderer *renderer) {
    SDL_Rect fillRect = {mPosX, mPosY, mWidth, mHeight};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}
