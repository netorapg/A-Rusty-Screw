#include "Object.h"

Object::Object(float x, float y, float width, float height)
    : mPosX(x), mPosY(y), mWidth(width), mHeight(height)
{
}

void Object::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
    // Implementação padrão, se necessário
}
