#include "../include/arscrew/Camera.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace ARSCREW
{
    Camera::Camera
    (
        float width, 
        float height
    ) : mSize(width, height), mPosition(0, 0), mShakeIntensity(0.0f), mShakeDuration(0.0f), mShakeTimer(0.0f) {}
    void Camera::move(Vector& offset)   
    {
        mPosition += offset;
        mPosition.x = std::max(0.0f, mPosition.x);
        mPosition.y = std::max(0.0f, mPosition.y);
    }
    SDL_Rect Camera::getSDLRect() const 
    {
        return 
        {
            static_cast<int>(mPosition.x),
            static_cast<int>(mPosition.y),
            static_cast<int>(mSize.x),
            static_cast<int>(mSize.y)
        };
    }
    void Camera::setPosition(const Vector& position) 
    {
        mPosition = position;

        mPosition.x = std::max(0.0f, mPosition.x);
        mPosition.y = std::max(0.0f, mPosition.y);
    }

    Vector Camera::getPosition() const 
    {
        return mPosition;
    }
    Vector Camera::getSize() const 
    {
        return mSize;
    }

    void Camera::shake(float intensity, float duration)
    {
        mShakeIntensity = intensity;
        mShakeDuration = duration;
        mShakeTimer = duration;
    }

    void Camera::update(float deltaTime)
    {
        if (mShakeTimer > 0.0f)
        {
            mShakeTimer -= deltaTime;
            if (mShakeTimer < 0.0f)
            {
                mShakeTimer = 0.0f;
                mShakeIntensity = 0.0f;
            }
        }
    }

    Vector Camera::getShakeOffset() const
    {
        if (mShakeTimer <= 0.0f || mShakeIntensity <= 0.0f)
        {
            return Vector(0.0f, 0.0f);
        }

    
        static bool seedInitialized = false;
        if (!seedInitialized)
        {
            srand(static_cast<unsigned int>(time(nullptr)));
            seedInitialized = true;
        }

        float currentIntensity = mShakeIntensity * (mShakeTimer / mShakeDuration);

        // Gerar offset aleatório usando rand()
        float offsetX = ((rand() % 2001) - 1000) / 1000.0f * currentIntensity; // -1.0 a 1.0
        float offsetY = ((rand() % 2001) - 1000) / 1000.0f * currentIntensity; // -1.0 a 1.0

        return Vector(offsetX, offsetY);
    }
}
