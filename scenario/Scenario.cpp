#include "Scenario.h"

Scenario::Scenario(float x, float y, float width, float height)
    : mPosX(x), mPosY(y), mWidth(width), mHeight(height)
{
}

bool Scenario::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight)
{
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth &&
            mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}
