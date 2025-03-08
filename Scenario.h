#ifndef SCENARIO_H
#define SCENARIO_H

#include <SDL2/SDL.h>

class Scenario
{
public:
    Scenario(float x, float y, float width, float height);

    virtual void render(SDL_Renderer *renderer, float cameraX, float cameraY) = 0; // Função virtual pura
    virtual bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight);

    float getX() const { return mPosX; }
    float getY() const { return mPosY; }
    float getWidth() const { return mWidth; }
    float getHeight() const { return mHeight; }

    void setX(float x) { mPosX = x; }
    void setY(float y) { mPosY = y; }

protected:
    float mPosX, mPosY, mWidth, mHeight;
};

#endif // SCENARIO_H
