#include "../include/bettlerider/SolidPlatform.h"

namespace BRTC
{

void SolidPlatform::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    SDL_Rect fillRect = {
        static_cast<int>(mX - cameraX),
        static_cast<int>(mY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x8B, 0xFF); // Cor para plataforma sólida
    SDL_RenderFillRect(renderer, &fillRect);
}

}