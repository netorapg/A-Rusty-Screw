#include "../include/bettlerider/Wall.h"

namespace BRTC
{

void Wall::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    SDL_Rect fillRect = {
        static_cast<int>(mX - cameraX),
        static_cast<int>(mY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}

}