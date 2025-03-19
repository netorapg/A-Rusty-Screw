#include "../include/bettlerider/Door.h"

namespace BRTC
{

std::string Door::getLevelToLoad() const {
    return mLevelToLoad;
}

void Door::render(Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    SDL_Rect doorRect = {
        static_cast<int>(screenPosition.x),
        static_cast<int>(screenPosition.y),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };

    SDL_SetRenderDrawColor(BRTC::renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(BRTC::renderer, &doorRect);
}
}