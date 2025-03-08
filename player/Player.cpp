// Player.cpp
#include "Player.h"
#include <iostream>
// Player.cpp

Player::Player(float x, float y, SDL_Renderer *renderer)
    : Entity(x, y, PLAYER_WIDTH, PLAYER_HEIGHT),  // Passando os parâmetros necessários para o construtor de Entity
      mPos(x, y), mQuit(false), mFacingRight(true), mFalling(false)
{
  // Carregar a textura
  SDL_Surface *loadedSurface = IMG_Load("/home/netorapg/projects/platfom2d/assets/bezourinha_correndo.png");
  if (loadedSurface == nullptr)
    std::cerr << "Failed to load player image: " << IMG_GetError() << std::endl;

  mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
  SDL_FreeSurface(loadedSurface);

  mVel = Mylib::Math::Vector2f(0, 0);
  mSpriteClip = {0, 0, PLAYER_WIDTH, PLAYER_HEIGHT};
  mCurrentFrame = 0;
  mFrameCount = 3;
  mAnimationTimer = 0.0f;
  mAnimationSpeed = 0.7f;
}

Player::~Player()
{
  SDL_DestroyTexture(mTexture);
}

void Player::reset()
{
  mPos.x = 25;
  mPos.y = 5;
  mVel.x = 0;
  mVel.y = 0;
  mFalling = false;
  mAttacking = false;
  mPassingThroughPlatform = false;
  mQuit = false;
  mFacingRight = true;
}

void Player::handleEvent(SDL_Event &e)
{
  if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
  {
    switch (e.key.keysym.sym)
    {
    case SDLK_d:
      mVel.x = 5;
      mFacingRight = true;
      break;
    case SDLK_a:
      mVel.x = -5;
      mFacingRight = false;
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
        mPassingThroughPlatform = true;
      break;
    }
  }
  else if (e.type == SDL_KEYUP)
  {
    switch (e.key.keysym.sym)
    {
    case SDLK_a:
    case SDLK_d:
      mVel.x = 0;
      break;
    case SDLK_s:
      mPassingThroughPlatform = false;
      break;
    }
  }
}

void Player::move(std::list<Platform> &platforms,
                  std::list<SolidPlatform> &solidPlatforms,
                  std::list<Wall> &walls,
                  std::list<Crate> &crates)
{
  Entity::move(platforms, solidPlatforms, walls, crates);
}

void Player::render(SDL_Renderer *renderer, float cameraX, float cameraY)
{
  SDL_Rect renderQuad = {static_cast<int>(mPos.x - cameraX),
                        static_cast<int>(mPos.y - cameraY),
                        PLAYER_WIDTH,
                        PLAYER_HEIGHT};

  if (mFacingRight)
  {
    SDL_RenderCopy(renderer, mTexture, &mSpriteClip, &renderQuad);
  }
  else
  {
    SDL_RenderCopyEx(renderer,
                     mTexture,
                     &mSpriteClip,
                     &renderQuad,
                     0,
                     nullptr,
                     SDL_FLIP_HORIZONTAL);
  }
}

int Player::getWidth() const
{
  return mSpriteClip.w;
}

int Player::getHeight() const
{
  return mSpriteClip.h;
}
