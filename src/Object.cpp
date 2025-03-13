#include "../include/bettlerider/Object.h"

bool BRTC::StaticObject::isVisible(float cameraX, float cameraY, float screenWidth, float screenHeight )
{
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth && mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}