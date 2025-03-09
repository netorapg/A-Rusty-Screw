#include "Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>

const int   PLAYER_WIDTH  = 25;
const int   PLAYER_HEIGHT = 26;
const float GRAVITY       = 0.5f;

Player::Player(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, PLAYER_WIDTH, PLAYER_HEIGHT),
      mFacingRight(true), mSpriteClip{0, 0, PLAYER_WIDTH, PLAYER_HEIGHT} {

    SDL_Surface* loadedSurface = IMG_Load("../assets/bezourinha_correndo.png");
    if (!loadedSurface) {
        std::cerr << "Failed to load player image: " << IMG_GetError() << std::endl;
    }
    mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    mSpriteClip = {0, 0, static_cast<int>(PLAYER_WIDTH), static_cast<int>(PLAYER_HEIGHT)};
    mCurrentFrame = 0;
    mFrameCount = 3;
    mAnimationTimer = 0.0f;
    mAnimationSpeed = 0.7f;
}

Player::~Player() {
    SDL_DestroyTexture(mTexture);
}

void Player::reset() {
    setPosition(25, 5);
    setVelocity(0, 0);
    mFalling = false;
    mPassingThroughPlatform = false;
    mFacingRight = true;
}

void Player::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_d:
                mVelX = 5;
                mFacingRight = true;
                break;
            case SDLK_a:
                mVelX = -5;
                mFacingRight = false;
                break;
            case SDLK_SPACE:
                if (!mFalling) {
                    mVelY = -10;
                    mFalling = true;
                }
                break;
            case SDLK_s:
                if (!mFalling)
                    mPassingThroughPlatform = true;
                break;
        }
    } else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_a:
            case SDLK_d:
                mVelX = 0;
                break;
            case SDLK_s:
                mPassingThroughPlatform = false;
                break;
        }
    }
}

void Player::move() {
    mVelY += GRAVITY;
    mPosX += mVelX;
    mPosY += mVelY;

    // Animação
    if (mVelX != 0) {
        mAnimationTimer += 0.1;
        if (mAnimationTimer >= mAnimationSpeed) {
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSpriteClip.x = mCurrentFrame * PLAYER_WIDTH;
            mAnimationTimer = 0;
        }
    } else {
        mCurrentFrame = 0;
    }
}

void Player::update() {
    mVelY += GRAVITY;
    mPosX += mVelX;
    mPosY += mVelY;

    // Atualizar animação
    if (mVelX != 0) {
        mAnimationTimer += 0.1;
        if (mAnimationTimer >= mAnimationSpeed) {
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSpriteClip.x = mCurrentFrame * PLAYER_WIDTH;
            mAnimationTimer = 0;
        }
    } else {
        mCurrentFrame = 0;
    }
}

void Player::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    SDL_Rect renderQuad = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT)
    };

    SDL_RendererFlip flip = mFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, mTexture, &mSpriteClip, &renderQuad, 0, nullptr, flip);
}

int Player::getWidth() const {
    return mSpriteClip.w;
}

int Player::getHeight() const {
    return mSpriteClip.h;
}
