#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

class Platform
{
public:
    Platform(int x, int y, int width, int height);

    void render(SDL_Renderer *renderer);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

private:
    int mPosX, mPosY;
    int mWidth, mHeight;
};

#endif // PLATFORM_H