#include "Player.h"
#include "../config.h"
#include <iostream>

const float PLAYER_SIZE = 40;
const float GRAVITY = 0.5;
const float ATTACK_WIDTH = 70;
const float ATTACK_HEIGHT = 20;

Player::Player(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls, std::list<Crate>& crates)
    : mPos(x, y), mVel(0, 0), mFalling(true), mAttacking(false), mPassingThroughPlatform(false), mPlatforms(platforms), mSolidPlatforms(solidPlatforms), mWalls(walls), mCrates(crates) {
    std::cout << "Player constructor called" << std::endl;
}

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
        case SDLK_s:
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
            mQuit = true; // Alterado para não chamar exit(0)
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

    // Check collision with normal platforms
    for (const auto &platform : mPlatforms)
    {
        if (!mPassingThroughPlatform &&
            checkCollision(mPos.x, mPos.y + PLAYER_SIZE, PLAYER_SIZE, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()) && mVel.y >= 0)
        {
            mPos.y = platform.getY() - PLAYER_SIZE;
            mFalling = false;
            mVel.y = 0;
            onPlatform = true;
            break;
        }
    }

    // Check collision with solid platforms
    for (const auto &solidPlatform : mSolidPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y + PLAYER_SIZE, PLAYER_SIZE, 1, solidPlatform.getX(), solidPlatform.getY(), solidPlatform.getWidth(), solidPlatform.getHeight()) && mVel.y >= 0)
        {
            mPos.y = solidPlatform.getY() - PLAYER_SIZE;
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

    // Check for collisions that would move the player vertically
    for (const auto &platform : mPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            if (mVel.y > 0 && !mPassingThroughPlatform)
            {
                mPos.y = platform.getY() - PLAYER_SIZE;
                mFalling = false;
                mVel.y = 0;
            }
        }
    }

    for (const auto &solidPlatform : mSolidPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE, solidPlatform.getX(), solidPlatform.getY(), solidPlatform.getWidth(), solidPlatform.getHeight()))
        {
            if (mVel.y > 0)
            {
                mPos.y = solidPlatform.getY() - PLAYER_SIZE;
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
        if (checkCollision(mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
        {
            if (mVel.x > 0) // Moving right
            {
                mPos.x = wall.getX() - PLAYER_SIZE;
                mVel.x = 0;
            }
            else if (mVel.x < 0) // Moving left
            {
                mPos.x = wall.getX() + wall.getWidth();
                mVel.x = 0;
            }
        }
    }

    for (auto &crate : mCrates)  // Precisa ser "auto &" para permitir alterar a posição
{
    if (checkCollision(mPos.x, mPos.y, PLAYER_SIZE, PLAYER_SIZE, crate.getX(), crate.getY(), crate.getWidth(), crate.getHeight()))
    {
        if (mVel.y > 0)  // Jogador caindo em cima do crate
        {
            mPos.y = crate.getY() - PLAYER_SIZE;
            mFalling = false;
            mVel.y = 0;
        }
        else if (mVel.y < 0)  // Jogador batendo no crate por baixo
        {
            mPos.y = crate.getY() + crate.getHeight();
            mVel.y = 0;
        }
        else if (mVel.x > 0)  // Jogador movendo o crate para a direita
        {
            crate.setX(crate.getX() + 5);  // Move o crate para a direita
        }
        else if (mVel.x < 0)  // Jogador movendo o crate para a esquerda
        {
            crate.setX(crate.getX() - 5);  // Move o crate para a esquerda
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
    SDL_Rect fillRect = {static_cast<int>(mPos.x), static_cast<int>(mPos.y), static_cast<int>(PLAYER_SIZE), static_cast<int>(PLAYER_SIZE)};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &fillRect);

    if (mAttacking)
    {
        SDL_Rect attackRect = {static_cast<int>(mAttackPos.x), static_cast<int>(mAttackPos.y), static_cast<int>(ATTACK_WIDTH), static_cast<int>(ATTACK_HEIGHT)};
        SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &attackRect);
    }
}

bool Player::checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}
