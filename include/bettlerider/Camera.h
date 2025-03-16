#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>
#include "../../my-lib-master/include/my-lib/math-vector.h"


namespace BRTC
{

using Vector = Mylib::Math::Vector<float, 2>;

class Camera
{
public:
    Camera(float width, float height);
    void move(Vector& offset);
    SDL_Rect getSDLRect() const;
    void setPosition(const Vector& position); // Alterado para const Vector&
    Vector getPosition() const;
    Vector getSize() const;

private:
    Vector mPosition;
    Vector mSize;
};
} // namespace BRTC
#endif // CAMERA_H