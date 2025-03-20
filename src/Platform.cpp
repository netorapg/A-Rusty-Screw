#include "../include/bettlerider/Platform.h"

namespace BRTC {

void Platform::render(SDL_Renderer* renderer, Vector cameraPosition)
{
    // Calcular posição relativa à câmera
    const Vector screenPosition = mPosition - cameraPosition;
    
    SDL_Rect fillRect = {
        static_cast<int>(screenPosition.x),
        static_cast<int>(screenPosition.y),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xCC, 0x80);
    SDL_RenderFillRect(renderer, &fillRect);
}

} // namespace BRTC