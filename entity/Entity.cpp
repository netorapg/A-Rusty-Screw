#include "Entity.h"

Entity::Entity(float x, float y, int width, int height)
    : mPosX(x), mPosY(y), mWidth(width), mHeight(height),
      mVelX(0), mVelY(0), mPassingThroughPlatform(false),
      mAboveCrate(false), mOnGround(false) {}


int Entity::getWidth() const {
    return mWidth;
}

int Entity::getHeight() const {
    return mHeight;
}

// Métodos para estados de colisão
void Entity::setOnGround(bool onGround) {
    mOnGround = onGround;
}

bool Entity::isOnGround() const {
    return mOnGround;
}

void Entity::setFalling(bool falling) {
    mFalling = falling;
}

bool Entity::isFalling() const {
    return mFalling;
}

float Entity::getPosX() const {
    return mPosX;
}

float Entity::getPosY() const {
    return mPosY;
}

void Entity::setPosition(float x, float y) {
    mPosX = x;
    mPosY = y;
}

float Entity::getHorizontalVelocity() const {
    return mVelX;
}

float Entity::getVerticalVelocity() const {
    return mVelY;
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

bool Entity::isPassingThroughPlatform() const {
    return mPassingThroughPlatform;
}

void Entity::setAboveCrate(bool aboveCrate) {
    mAboveCrate = aboveCrate;
}
