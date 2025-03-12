#include "../include/bettlerider/SolidPlatform.h"

namespace BRTC
{

SolidPlatform::SolidPlatform(float x, float y, float width, float height) : Scenario(x, y, width, height) {}

void SolidPlatform::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x8B, 0xFF); // Cor para plataforma sólida
    SDL_RenderFillRect(renderer, &fillRect);
}

}