#include "SolidPlatform.h"

SolidPlatform::SolidPlatform(float x, float y, float width, float height) : Object(x, y, width, height) {}

void SolidPlatform::render(SDL_Renderer *renderer, float cameraX, float cameraY) {
    // Calcule a posição relativa à câmera
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}

bool SolidPlatform::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) {
    // Verifica se a plataforma sólida está visível na tela
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth &&
            mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}
