#include "../include/bettlerider/SolidPlatform.h"

namespace BRTC
{

void SolidPlatform::render(SDL_Renderer* renderer, Vector cameraPosition)
{
    const Vector screenPosition = mPosition - cameraPosition;
    SDL_Rect fillRect = {
        static_cast<int>(screenPosition.x),
        static_cast<int>(screenPosition.y),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x8B, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}

}