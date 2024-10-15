// Object.h
#ifndef OBJECT_H
#define OBJECT_H

#include <SDL2/SDL.h>

class Object
{
public:
    Object(float x, float y, float width, float height);

    // Agora a função render aceita a posição da câmera
    virtual void render(SDL_Renderer *renderer, float cameraX, float cameraY) = 0; // Função virtual pura

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
