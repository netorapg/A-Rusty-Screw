#ifndef OBJECT_H
#define OBJECT_H

#include <SDL2/SDL.h>
#include <vector>

class Object
{
public:
    virtual void render(SDL_Renderer *renderer) = 0;
    virtual bool checkCollision(const SDL_Rect &otherRect) const = 0;
    virtual int getX() const = 0;
    virtual int getY() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
}

#endif 