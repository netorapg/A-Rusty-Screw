#include "Player.h"
#include <iostream>

const int PLAYER_WIDTH = 25;
const int PLAYER_HEIGHT = 26;
const float GRAVITY = 0.5f;


Player::Player(float x, float y, SDL_Renderer* renderer)
    : mPos(x, y), mQuit(false), mFacingRight(true), mFalling(false)
{
    // Carregar a textura do jogador (substitua pelo caminho correto da sua imagem)
    SDL_Surface* loadedSurface = IMG_Load("/home/netorapg/projects/platfom2d/assets/bezourinha_correndo.png");
    if (loadedSurface == nullptr)
    {
        std::cerr << "Failed to load player image: " << IMG_GetError() << std::endl;
    }
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

void Player::handleEvent(SDL_Event& e)
{
    // Movimento horizontal
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
            if(!mFalling) {
                mPassingThroughPlatform = true;
            }
            break;
        }

    }
    else if (e.type == SDL_KEYUP)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_a:
        case SDLK_d:
            mVel.x = 0; // Zere a velocidade horizontal ao soltar a tecla
            break;
        case SDLK_s:
            mPassingThroughPlatform = false;
            break;
        }
    }
}


void Player::move() {
    // Apenas aplica movimento básico SEM FÍSICA
    mVel.y += GRAVITY;
    mPos.x += mVel.x;
    mPos.y += mVel.y;

    // Animação (mantida aqui)
    if(mVel.x != 0) {
        mAnimationTimer += 0.1;
        if(mAnimationTimer >= mAnimationSpeed) {
            mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
            mSpriteClip.x = mCurrentFrame * PLAYER_WIDTH;
            mAnimationTimer = 0;
        }
    } else {
        mCurrentFrame = 0;
    }
}


void Player::render(SDL_Renderer* renderer, float cameraX, float cameraY) // Atualizado para incluir câmera
{
    SDL_Rect renderQuad = {
        static_cast<int>(mPos.x - cameraX), // Ajusta a posição pela câmera
        static_cast<int>(mPos.y - cameraY), // Ajusta a posição pela câmera
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };

    // Renderizar o jogador, ajustando o flip dependendo da direção
    if (mFacingRight)
    {
        SDL_RenderCopy(renderer, mTexture, &mSpriteClip, &renderQuad);
    }
    else
    {
        SDL_RenderCopyEx(renderer, mTexture, &mSpriteClip, &renderQuad, 0, nullptr, SDL_FLIP_HORIZONTAL);
    }
}
int Player::getWidth() const {
    return mSpriteClip.w;  // Retorna a largura do sprite
}

int Player::getHeight() const {
    return mSpriteClip.h;  // Retorna a altura do sprite
}
