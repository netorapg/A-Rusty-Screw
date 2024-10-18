#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Platform::render(SDL_Renderer *renderer, float cameraX, float cameraY) {
    // Calcule a posição relativa à câmera
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0xCC, 0x80);  // Azul menos claro com 50% de transparência




    SDL_RenderFillRect(renderer, &fillRect);
}

bool Platform::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) {
    // Verifica se a plataforma está visível na tela
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth &&
            mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}
