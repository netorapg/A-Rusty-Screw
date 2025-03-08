#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) : Scenario(x, y, width, height) {}

void Platform::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xCC, 0x80); // Cor para plataforma
    SDL_RenderFillRect(renderer, &fillRect);
}
