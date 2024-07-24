#include "Player.h"
#include "config.h"


const int PLAYER_SIZE = 40;
const int GRAVITY = 1;
const int ATTACK_WIDTH = 70;
const int ATTACK_HEIGHT = 20;

Player::Player(int x, int y, std::vector<Platform> &platforms) : mPos(x, y), mVel(0, 0), mFalling(true), mAttacking(false), mPlatforms(platforms) {}

void Player::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_d:
            if(!mFalling){
            mVel.x = 5;
            }
            break;
        case SDLK_a:
            if(!mFalling){
            mVel.x = -5;
            }
            break;
        case SDLK_SPACE:
            if (!mFalling)
            {
                mVel.y = -15;
                mFalling = true;
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
            if(mVel.x > 0){
            mVel.x = 0;
            }
            break;
        case SDLK_a:
            if(mVel.x < 0){
            mVel.x = 0;
            }
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
   

    if (checkCollision(mPlatforms))
    {
        mPos.y -= 0;
        mVel.y = 0;
        mFalling = false;
    }
    else if (!checkCollision(mPlatforms) && mPos.y + PLAYER_SIZE < SCREEN_HEIGHT)
    {
        mFalling = true;
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

bool Player::checkCollision(std::vector<Platform> &platforms)
{
    SDL_Rect playerRect = {mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE};

    for (const auto &platform : platforms)
    {
        SDL_Rect platformRect = {platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()};
        if (checkCollision(playerRect, platformRect))
        {

            return true;
        }
    }

    return false;
}

bool Player::checkCollision(const SDL_Rect &a, const SDL_Rect &b)
{
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}