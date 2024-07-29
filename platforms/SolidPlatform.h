#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include <SDL2/SDL.h>

class SolidPlatform
{
public:
    SolidPlatform(int x, int y, int width, int height);

    void render(SDL_Renderer *renderer);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

private:
    int mPosX, mPosY;
    int mWidth, mHeight;
};

#endif // SOLIDPLATFORM_H