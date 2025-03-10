#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>

class Camera {
public:
    Camera(int width, int height);

    void move(int x, int y);
    SDL_Rect getCamera() const;

    // Métodos que você deve adicionar
    void setPosition(int x, int y);
    int getX() const;
    int getY() const;

private:
    SDL_Rect mCamera;
    int mWidth;
    int mHeight;
};

#endif // CAMERA_H
