// Wall.cpp
#include "Wall.h"

Wall::Wall(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Wall::render(SDL_Renderer *renderer, float cameraX, float cameraY) {
    // Calcule a posição relativa à câmera
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black color for walls
    SDL_RenderFillRect(renderer, &fillRect);
}
