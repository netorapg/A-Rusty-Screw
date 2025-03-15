#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

namespace BRTC
{

class Camera {
    public:
        Camera(float width, float height);
    
        void move(float x, float y);
        SDL_Rect getCamera() const;
    
    
        void setPosition(float x, float y);
        float getX() const;
        float getY() const;
    
    private:
        SDL_Rect mCamera;
        float mWidth;
        float mHeight;
    };
}

#endif // CAMERA_H
