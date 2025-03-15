#include "../include/bettlerider/Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>

const int PLAYER_WIDTH = 25;
const int PLAYER_HEIGHT = 26;
const float GRAVITY = 0.5f;

namespace BRTC {

Player::Player(float x, float y, SDL_Renderer* renderer)
: DynamicObject(x, y, PLAYER_WIDTH, PLAYER_HEIGHT),
  mFacingRight(true), mSpriteClip{0, 0, PLAYER_WIDTH, PLAYER_HEIGHT}
{
    SDL_Surface* loadedSurface = IMG_Load("../assets/bezourinha_correndo.png");
    if (!loadedSurface) {
        std::cerr << "Failed to load player image: " << IMG_GetError() << std::endl;
    }
    mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);

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
    setFalling(false);
    setPassingThroughPlatform(false);
    mFacingRight = true;
}

void Player::handleEvent(SDL_Event& e) 
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_d:
                setHorizontalVelocity(5); // Usa setter
                mFacingRight = true;
                break;
            case SDLK_a:
                setHorizontalVelocity(-5);
                mFacingRight = false;
                break;
            case SDLK_SPACE:
                if (isOnGround()) { // Usa getter de DynamicObject
                    setVerticalVelocity(-10);
                    setFalling(true);
                }
                break;
            case SDLK_s:
                if (isOnGround())
                    setPassingThroughPlatform(true);
                break;
        }
    } else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_a:
            case SDLK_d:
                setHorizontalVelocity(0);
                break;
            case SDLK_s:
                setPassingThroughPlatform(false);
                break;
        }
    }
}

void Player::update() {
    setVerticalVelocity(getVerticalVelocity() + GRAVITY);
    setPosition(getX() + getHorizontalVelocity(), getY() + getVerticalVelocity());
    
    // Animação
    if (getHorizontalVelocity() != 0) {
        mAnimationTimer += 0.1f;
        if (mAnimationTimer >= mAnimationSpeed) {
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSpriteClip.x = mCurrentFrame * PLAYER_WIDTH;
            mAnimationTimer = 0.0f;
        }
    } else {
        mCurrentFrame = 0;
    }
}

void Player::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    SDL_Rect renderQuad = {
        static_cast<int>(getX() - cameraX),
        static_cast<int>(getY() - cameraY),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };

    SDL_RendererFlip flip = mFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, mTexture, &mSpriteClip, &renderQuad, 0, nullptr, flip);
}


} // namespace BRTC