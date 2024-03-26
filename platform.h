#include <SDL2/SDL.h>
#ifndef PLATFORM_H // Verifica se a macro PLATFORM_H não está definida
#define PLATFORM_H // Define a macro PLATFORM_H para evitar múltiplas definições
class Platform
{
public:
    int mPosX, mPosY;
    int mWidth, mHeight;
    Platform(int x, int y, int width, int height) : mPosX(x), mPosY(y), mWidth(width), mHeight(height) {}

    void render(SDL_Renderer *renderer)
    {
        SDL_Rect fillRect = {mPosX, mPosY, mWidth, mHeight};
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);
    }

private:
};
#endif // Fim da inclusão de guarda