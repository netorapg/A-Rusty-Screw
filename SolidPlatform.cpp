#include "SolidPlatform.h"

SolidPlatform::SolidPlatform(int x, int y, int width, int height)
    : Platform(x, y, width, height) {}

void SolidPlatform::render(SDL_Renderer *renderer)
{
    SDL_Rect fillRect = { getX(), getY(), getWidth(), getHeight() };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF); // Cor diferente
    SDL_RenderFillRect(renderer, &fillRect);
}
