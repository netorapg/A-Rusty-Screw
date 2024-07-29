#ifndef OBJECT_H
#define OBJECT_H
#include <SDL2/SDL.h>
class Object
{
public:
    Object(float x, float y, float width, float height);

    void render(SDL_Renderer *renderer);

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    void setX(float x);
    void setY(float y);
    void setWidth(float width);
    void setHeight(float height);
    float mPosX, mPosY;
    float mWidth, mHeight;
  
};

#endif // OBJECT_H