#include <SDL2/SDL.h>
#include "config.h"
#include "platform.h"
#include <vector>
const int PLAYER_SIZE = 40;
const int GRAVITY = 1;
const int ATTACK_WIDTH = 70;
const int ATTACK_HEIGHT = 20;

class Player
{
public:
    int mVelX, mVelY;
    int mPosX, mPosY;
    Player(int x, int y) : mPosX(x), mPosY(y), mVelX(0), mVelY(0), mFalling(true) {}

    void handleEvent(SDL_Event &e)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_d:
                mVelX = 5;
                break;
            case SDLK_a:
                mVelX = -5;
                break;
            case SDLK_SPACE:
                if (!mFalling)
                {
                    mVelY = -15;
                    mFalling = true;
                }
                break;
            case SDLK_j:
                if (!mAttacking)
                {
                    mAttacking = true;
                    mAttackPosX = mPosX + PLAYER_SIZE;
                    mAttackPosY = mPosY + PLAYER_SIZE / 2 - ATTACK_HEIGHT / 2;
                }
                break;

            case SDLK_ESCAPE:
                exit(0);
                break;
            }
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_d:
                mVelX = 0;
                break;
            case SDLK_a:
                mVelX = 0;
                break;

            case SDLK_j:
                mAttacking = false;
                break;
            }
        }
    }

    void move()
    {
        mPosX += mVelX;

        if (mFalling)
        {
            mPosY += mVelY;
            mVelY += GRAVITY;
        }

        if (mPosY + PLAYER_SIZE >= SCREEN_HEIGHT)
        {
            mPosY = SCREEN_HEIGHT - PLAYER_SIZE;
            mFalling = false;
        }

        if (mAttacking)
        {
            mAttackPosX += mVelX;
            if (mAttackPosX > SCREEN_WIDTH)
            {
                mAttacking = false;
            }
        }
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_Rect fillRect = {mPosX, mPosY, PLAYER_SIZE, PLAYER_SIZE};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);

        if (mAttacking)
        {
            SDL_Rect attackRect = {mAttackPosX, mAttackPosY, ATTACK_WIDTH, ATTACK_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderFillRect(renderer, &attackRect);
        }
    }

    bool checkCollision(const std::vector<Platform> &platforms)
    {
        SDL_Rect playerRect = {mPosX, mPosY, PLAYER_SIZE, PLAYER_SIZE};

        for (const auto &platform : platforms)
        {
            SDL_Rect platformRect = {platform.mPosX, platform.mPosY, platform.mWidth, platform.mHeight};
            if (checkCollision(playerRect, platformRect))
            {
                return true;
            }
        }

        return false;
    }

private:
    bool mFalling;
    bool mAttacking;
    int mAttackPosX, mAttackPosY;
    bool checkCollision(const SDL_Rect &a, const SDL_Rect &b)
    {
        return (a.x < b.x + b.w &&
                a.x + a.w > b.x &&
                a.y < b.y + b.h &&
                a.y + a.h > b.y);
    }
};