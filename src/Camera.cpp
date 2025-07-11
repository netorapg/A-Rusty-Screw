#include "../include/arscrew/Camera.h"

namespace ARSCREW
{
    Camera::Camera
    (
        float width, 
        float height
    ) : mSize(width, height), mPosition(0, 0) {}
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
}
