#include "Player.h"
#include "config.h"

const float PLAYER_SIZE = 40;
const float GRAVITY = 0.5;
const float ATTACK_WIDTH = 70;
const float ATTACK_HEIGHT = 20;

Player::Player(float x, float y, std::vector<Platform> &platforms) : mPos(x, y), mVel(0, 0), mFalling(true), mAttacking(false), mPassingThroughPlatform(false), mPlatforms(platforms) {}

void Player::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_d:
            mVel.x = 5;
            break;
        case SDLK_a:
            mVel.x = -5;
            break;
        case SDLK_SPACE:
            if (!mFalling)
            {
                mVel.y = -10;
                mFalling = true;
            }
            break;
        case SDLK_s: // Botão de descer
            if (!mFalling)
            {
                mPassingThroughPlatform = true;
            }
            break;
        case SDLK_j:
            if (!mAttacking)
            {
                mAttacking = true;
                mAttackPos.set(mPos.x + PLAYER_SIZE, mPos.y + PLAYER_SIZE / 2 - ATTACK_HEIGHT / 2);
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
            if (mVel.x > 0)
            {
                mVel.x = 0;
            }
            break;
        case SDLK_a:
            if (mVel.x < 0)
            {
                mVel.x = 0;
            }
            break;
        case SDLK_s:
            mPassingThroughPlatform = false;
            break;
        case SDLK_j:
            mAttacking = false;
            break;
        }
    }
}

void Player::move()
{
    mPos += mVel;

    if (mFalling)
    {
        mPos.y += mVel.y;
        mVel.y += GRAVITY;
    }

    if (mPos.y + PLAYER_SIZE >= SCREEN_HEIGHT)
    {
        mPos.y = SCREEN_HEIGHT - PLAYER_SIZE;
        mFalling = false;
        mVel.y = 0;
    }

    bool onPlatform = false;

    for (const auto &platform : mPlatforms)
    {
        if (!mPassingThroughPlatform &&
            checkCollision(mPos.x, mPos.y + PLAYER_SIZE, PLAYER_SIZE, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            mPos.y = platform.getY() - PLAYER_SIZE;
            mFalling = false;
            mVel.y = 0;
            onPlatform = true;
            break;
        }
    }

    if (!onPlatform && mPos.y + PLAYER_SIZE < SCREEN_HEIGHT)
    {
        mFalling = true;
    }

    for (const auto &platform : mPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            // Evitar passar pela plataforma por baixo, a menos que o jogador esteja atravessando a plataforma
            if (mVel.y > 0 && !mPassingThroughPlatform)
            {
                mPos.y = platform.getY() - PLAYER_SIZE;
                mFalling = false;
                mVel.y = 0;
            }
        }
    }

    if (mAttacking)
    {
        mAttackPos.x += mVel.x;
        if (mAttackPos.x > SCREEN_WIDTH)
        {
            mAttacking = false;
        }
    }
}

void Player::render(SDL_Renderer *renderer)
{
    SDL_Rect fillRect = {mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);

    if (mAttacking)
    {
        SDL_Rect attackRect = {mAttackPos.x, mAttackPos.y, ATTACK_WIDTH, ATTACK_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &attackRect);
    }
}

bool Player::checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}