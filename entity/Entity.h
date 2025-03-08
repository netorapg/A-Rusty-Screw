// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <list>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"

const float GRAVITY       = 0.5f;

class Entity {
public:
    Entity(float x, float y, float width, float height)
        : mPosX(x), mPosY(y), mWidth(width), mHeight(height), mVelX(0), mVelY(0), mOnGround(false), mFalling(false) {}

    virtual ~Entity() {}

    // Getters and setters
    float getPosX() const { return mPosX; }
    float getPosY() const { return mPosY; }
    float getVelX() const { return mVelX; }
    float getVelY() const { return mVelY; }

    void setVelX(float velX) { mVelX = velX; }
    void setVelY(float velY) { mVelY = velY; }

    bool isOnGround() const { return mOnGround; }
    bool isFalling() const { return mFalling; }

    // Movimento genérico
    virtual void move(std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, 
                      std::list<Wall>& walls, std::list<Crate>& crates);

protected:
    float mPosX, mPosY;
    float mWidth, mHeight;
    float mVelX, mVelY;
    bool mOnGround;
    bool mFalling;

    bool checkCollision(float x1, float y1, float w1, float h1, 
                        float x2, float y2, float w2, float h2);
};

#endif // ENTITY_H
