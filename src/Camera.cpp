#include "../include/bettlerider/Camera.h"

namespace BRTC
{

Camera::Camera(int width, int height)
: mWidth(width), mHeight(height) {
mCamera.x = 0;
mCamera.y = 0;
mCamera.w = width;
mCamera.h = height;
}

void Camera::move(int x, int y) {
mCamera.x += x;
mCamera.y += y;

// Impede que a câmera saia dos limites do mundo
if (mCamera.x < 0) {
    mCamera.x = 0;
}
if (mCamera.y < 0) {
    mCamera.y = 0;
}
}

SDL_Rect Camera::getCamera() const {
return mCamera;
}

// Implementação dos métodos getX, getY e setPosition
int Camera::getX() const {
return mCamera.x;
}

int Camera::getY() const {
return mCamera.y;
}

void Camera::setPosition(int x, int y) {
mCamera.x = x;
mCamera.y = y;
}
}