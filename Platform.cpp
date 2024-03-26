#include "Platform.h"

Platform::Platform(int x, int y, int width, int height) : mPosX(x), mPosY(y), mWidth(width), mHeight(height) {}

void Platform::render(SDL_Renderer *renderer) {
    SDL_Rect fillRect = {mPosX, mPosY, mWidth, mHeight};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}

int Platform::getX() const {
    return mPosX;
}

int Platform::getY() const {
    return mPosY;
}

int Platform::getWidth() const {
    return mWidth;
}

int Platform::getHeight() const {
    return mHeight;
}

