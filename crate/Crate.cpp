#include "Crate.h"
#include "../config.h"
#include <iostream>

const float CRATE_SIZE = 60;
const float GRAVITY = 0.5;

Crate::Crate(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls)
    : mVel(0, 0), mPos(x, y), mFalling(true), mQuit(false),
      mPlatforms(platforms), mSolidPlatforms(solidPlatforms), mWalls(walls)
{
    std::cout << "Crate constructor called" << std::endl;
}

void Crate::move() {
    mPos += mVel;

    if(mFalling) {
        mPos.y += mVel.y;
        mVel.y += GRAVITY;
    }

    if (mPos.y + CRATE_SIZE > SCREEN_HEIGHT) {
        mPos.y = SCREEN_HEIGHT - CRATE_SIZE;
        mVel.y = 0;
        mFalling = false;
    }

    bool onPlatform = false;
    for (auto &platform : mPlatforms) {
        if (checkCollision(mPos.x, mPos.y, CRATE_SIZE, CRATE_SIZE, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight())) {
            mPos.y = platform.getY() - CRATE_SIZE;
            mVel.y = 0;
            mFalling = false;
            onPlatform = true;
        }
    }

    for (auto &solidPlatform : mSolidPlatforms) {
        if (checkCollision(mPos.x, mPos.y, CRATE_SIZE, CRATE_SIZE, solidPlatform.getX(), solidPlatform.getY(), solidPlatform.getWidth(), solidPlatform.getHeight())) {
            mPos.y = solidPlatform.getY() - CRATE_SIZE;
            mVel.y = 0;
            mFalling = false;
            onPlatform = true;
            break;
        }
    }
for (const auto &platform : mPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, CRATE_SIZE, CRATE_SIZE, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            if (mVel.y > 0)
            {
                mPos.y = platform.getY() - CRATE_SIZE;
                mFalling = false;
                mVel.y = 0;
            }
        }
    }

    for (const auto &solidPlatform : mSolidPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, CRATE_SIZE, CRATE_SIZE, solidPlatform.getX(), solidPlatform.getY(), solidPlatform.getWidth(), solidPlatform.getHeight()))
        {
            if (mVel.y > 0)
            {
                mPos.y = solidPlatform.getY() - CRATE_SIZE;
                mFalling = false;
                mVel.y = 0;
            }
            else if (mVel.y < 0)
            {
                mPos.y = solidPlatform.getY() + solidPlatform.getHeight();
                mVel.y = 0;
            }
        }
    }

    for (const auto &wall : mWalls)
    {
        if (checkCollision(mPos.x, mPos.y, CRATE_SIZE, CRATE_SIZE, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
        {
            if (mVel.x > 0) // Moving right
            {
                mPos.x = wall.getX() - CRATE_SIZE;
                mVel.x = 0;
            }
            else if (mVel.x < 0) // Moving left
            {
                mPos.x = wall.getX() + wall.getWidth();
                mVel.x = 0;
            }
        }
    }
}
    void Crate::render(SDL_Renderer *renderer) {
         SDL_Rect fillRect = {static_cast<int>(mPos.x), static_cast<int>(mPos.y), static_cast<int>(CRATE_SIZE), static_cast<int>(CRATE_SIZE)};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);
    }
    
    bool Crate::checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
         return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}