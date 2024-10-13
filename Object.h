#ifndef OBJECT_H
#define OBJECT_H

#include <SDL2/SDL.h>

class Object
{
public:
    Object(float x, float y, float width, float height);

    virtual void render(SDL_Renderer *renderer) = 0; // Função virtual pura

    float getX() const { return mPosX; }
    float getY() const { return mPosY; }
    float getWidth() const { return mWidth; }
    float getHeight() const { return mHeight; }

    void setX(float x) { mPosX = x; }
    void setY(float y) { mPosY = y; }

protected:
    float mPosX, mPosY, mWidth, mHeight;
};

#endif // OBJECT_H
