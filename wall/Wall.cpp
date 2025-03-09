#include "Wall.h"

Wall::Wall(float x, float y, float width, float height) : Scenario(x, y, width, height) {}

void Wall::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Cor preta para as paredes
    SDL_RenderFillRect(renderer, &fillRect);
}

bool Wall::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight)
{
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth &&
            mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}
