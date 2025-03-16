#include "../include/bettlerider/Player.h"
#include <SDL2/SDL_image.h>
#include <iostream>

namespace BRTC {

Player::Player(Vector position, SDL_Renderer* renderer)
    : DynamicObject(position, Vector(25, 26)),  // Tamanho 25x26
      mFacingRight(true),
      mSpriteClip{0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y)}
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

void Player::handleEvent(SDL_Event& e) 
{
    Vector velocity = getVelocity();

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_d:
                velocity.x = 5.0f;
                mFacingRight = true;
                break;
            case SDLK_a:
                velocity.x = -5.0f;
                mFacingRight = false;
                break;
            case SDLK_SPACE:
                if (isOnGround()) {
                    velocity.y = -10.0f;
                    setFalling(true);
                }
                break;
            case SDLK_s:
                if (isOnGround())
                    setPassingThroughPlatform(true);
                break;
        }
    } 
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_a:
            case SDLK_d:
                velocity.x = 0.0f;
                break;
            case SDLK_s:
                setPassingThroughPlatform(false);
                break;
        }
    }

    setVelocity(velocity);
}

void Player::update(float deltaTime) 
{
    // Aplicar física com deltaTime
    Vector velocity = getVelocity();
    Vector position = getPosition();

    velocity.y += GRAVITY * deltaTime;
    position += velocity * deltaTime;

    setVelocity(velocity);
    setPosition(position);

    // Animação
    if (velocity.x != 0.0f) {
        mAnimationTimer += deltaTime;
        if (mAnimationTimer >= mAnimationSpeed) {
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSpriteClip.x = mCurrentFrame * static_cast<int>(mSize.x);
            mAnimationTimer = 0.0f;
        }
    } else {
        mCurrentFrame = 0;
    }
}

void Player::render(SDL_Renderer* renderer, Vector cameraPosition) 
{
    Vector screenPosition = getPosition() - cameraPosition;
    
    SDL_Rect renderQuad = {
        static_cast<int>(screenPosition.x),
        static_cast<int>(screenPosition.y),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };

    SDL_RendererFlip flip = mFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, mTexture, &mSpriteClip, &renderQuad, 0, nullptr, flip);
}

void Player::setPassingThroughPlatform(bool enable) 
{
    mPassingThroughPlatform = enable;
}

} // namespace BRTC