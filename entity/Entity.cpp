#include "Entity.h"

Entity::Entity(float x, float y, int width, int height)
    : mPosX(x), mPosY(y), mWidth(width), mHeight(height),
      mVelX(0), mVelY(0), mPassingThroughPlatform(false),
      mAboveCrate(false), mOnGround(false) {}

// Getters
int Entity::getWidth() const {
    return mWidth;
}

int Entity::getHeight() const {
    return mHeight;
}

float Entity::getPosX() const {
    return mPosX;
}

float Entity::getPosY() const {
    return mPosY;
}

float Entity::getHorizontalVelocity() const {
    return mVelX;
}

float Entity::getVerticalVelocity() const {
    return mVelY;
}

bool Entity::isOnGround() const {
    return mOnGround;
}

bool Entity::isFalling() const {
    return mFalling;
}

bool Entity::isPassingThroughPlatform() const {
    return mPassingThroughPlatform;
}

// Setters
void Entity::setPosition(float x, float y) {
    mPosX = x;
    mPosY = y;
}

void Entity::setVelocity(float vx, float vy) {
    mVelX = vx;
    mVelY = vy;
}

void Entity::setHorizontalVelocity(float vx) {
    mVelX = vx;
}

void Entity::setVerticalVelocity(float vy) {
    mVelY = vy;
}

void Entity::setOnGround(bool onGround) {
    mOnGround = onGround;
}

void Entity::setFalling(bool falling) {
    mFalling = falling;
}

void Entity::setAboveCrate(bool aboveCrate) {
    mAboveCrate = aboveCrate;
}
