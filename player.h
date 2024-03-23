#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SQUARE_SIZE = 40;
const int GRAVITY = 1;
const int ATTACK_WIDTH = 70;
const int ATTACK_HEIGHT = 20;

class Player
{
public:
    Player(int x, int y) : mPosX(x), mPosY(y), mVelX(0), mVelY(0), mFalling(true) {}

    void handleEvent(SDL_Event &e)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_RIGHT:
                mVelX = 5;
                break;
            case SDLK_LEFT:
                mVelX = -5;
                break;
            case SDLK_SPACE:
                if (!mFalling)
                {
                    mVelY = -15;
                    mFalling = true;
                }
                break;
            case SDLK_z:
                if (!mFalling & !mAttacking)
                {
                    mAttacking = true;
                    mAttackPosX = mPosX + SQUARE_SIZE;
                    mAttackPosY = mPosY + SQUARE_SIZE / 2 - ATTACK_HEIGHT / 2;
                }
                break;
            }
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_RIGHT:
                mVelX = 0;
                break;
            case SDLK_LEFT:
                mVelX = 0;
                break;

            case SDLK_z:
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

        if (mPosY + SQUARE_SIZE >= SCREEN_HEIGHT)
        {
            mPosY = SCREEN_HEIGHT - SQUARE_SIZE;
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
        SDL_Rect fillRect = {mPosX, mPosY, SQUARE_SIZE, SQUARE_SIZE};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);

        if (mAttacking)
        {
            SDL_Rect attackRect = {mAttackPosX, mAttackPosY, ATTACK_WIDTH, ATTACK_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderFillRect(renderer, &attackRect);
        }
    }

private:
    int mPosX, mPosY;
    int mVelX, mVelY;
    bool mFalling;
    bool mAttacking;
    int mAttackPosX, mAttackPosY;
};