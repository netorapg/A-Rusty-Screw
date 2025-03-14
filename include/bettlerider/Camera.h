#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

namespace BRTC
{

class Camera {
    public:
        Camera(int width, int height);
    
        void move(int x, int y);
        SDL_Rect getCamera() const;
    
    
        void setPosition(int x, int y);
        int getX() const;
        int getY() const;
    
    private:
        SDL_Rect mCamera;
        int mWidth;
        int mHeight;
    };
}

#endif // CAMERA_H
