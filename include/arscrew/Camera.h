#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include "Globals.h"

namespace ARSCREW
{
    class Camera
    {
        public:
            SDL_Rect getSDLRect() const;
            Camera(float width, float height);
            void move(Vector& offset);
            void setPosition(const Vector& position);
            Vector getPosition() const;
            Vector getSize() const;
            void shake(float intensity, float duration);
            void update(float deltaTime);
            Vector getShakeOffset() const;
        private:
            Vector mPosition;
            Vector mSize;
            float mShakeIntensity;
            float mShakeDuration;
            float mShakeTimer;
    };
}
#endif
