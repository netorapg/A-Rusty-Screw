#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>
#include "Globals.h"


namespace BRTC
{


class Camera
{
public:
    Camera(float width, float height);
    void move(Vector& offset);
    SDL_Rect getSDLRect() const;
    void setPosition(const Vector& position);
    Vector getPosition() const;
    Vector getSize() const;

private:
    Vector mPosition;
    Vector mSize;
};
} // namespace BRTC
#endif // CAMERA_H