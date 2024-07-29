#include "Object.h"

Object::Object(float x, float y, float width, float height) : mPosX(x), mPosY(y), mWidth(width), mHeight(height) {}

void Object::render(SDL_Renderer *renderer) {
    SDL_Rect fillRect = {mPosX, mPosY, mWidth, mHeight};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);
}

float Object::getX() const {
    return mPosX;
}

float Object::getY() const {
    return mPosY;
}

float Object::getWidth() const {
    return mWidth;
}

float Object::getHeight() const {
    return mHeight;
}

void Object::setX(float x) {
    mPosX = x;
}

void Object::setY(float y) {
    mPosY = y;
}

void Object::setWidth(float width) {
    mWidth = width;
}

void Object::setHeight(float height) {
    mHeight = height;
}

