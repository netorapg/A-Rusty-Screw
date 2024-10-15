#include "Player.h"
#include <iostream>

const int PLAYER_WIDTH = 25;
const int PLAYER_HEIGHT = 26;
const float PLAYER_SPEED = 5;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = -10.0f;

Player::Player(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls, std::list<Crate>& crates, SDL_Renderer* renderer)
    : mPos(x, y), mPlatforms(platforms), mSolidPlatforms(solidPlatforms), mWalls(walls), mCrates(crates), mQuit(false), mFacingRight(true), mFalling(false)
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
    mFrameCount = 1;
    mAnimationTimer = 0.0f;
    mAnimationSpeed = 0.1f;
}

Player::~Player()
{
    SDL_DestroyTexture(mTexture);
}

void Player::handleEvent(SDL_Event& e)
{
    // Movimento horizontal
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_a:
            mVel.x = -PLAYER_SPEED; // Defina a velocidade negativa para mover para a esquerda
            mFacingRight = false;
            break;
        case SDLK_d:
            mVel.x = PLAYER_SPEED; // Defina a velocidade positiva para mover para a direita
            mFacingRight = true;
            break;
        case SDLK_SPACE:
            if (!mFalling)
            {
                mVel.y = JUMP_VELOCITY;
                mFalling = true;
            }
            break;
        }
    }
    else if (e.type == SDL_KEYUP)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_LEFT:
        case SDLK_RIGHT:
            mVel.x = 0; // Zere a velocidade horizontal ao soltar a tecla
            break;
        }
    }
}


void Player::move()
{
    // Gravidade
    mVel.y += GRAVITY;

    // Atualizar posição horizontal
    mPos.x += mVel.x;

    // Checagem de colisão horizontal (plataformas, paredes, etc.)
    for (const auto& wall : mWalls)
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_WIDTH, PLAYER_HEIGHT, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
        {
            // Colisão detectada - ajustar posição
            if (mVel.x > 0) // Movendo para a direita
            {
                mPos.x = wall.getX() - PLAYER_WIDTH; // Ajustar a posição para a esquerda do wall
            }
            else if (mVel.x < 0) // Movendo para a esquerda
            {
                mPos.x = wall.getX() + wall.getWidth(); // Ajustar a posição para a direita do wall
            }
            mVel.x = 0; // Parar o movimento horizontal
        }
    }

    // Atualizar posição vertical
    mPos.y += mVel.y;

    // Checagem de colisão vertical (plataformas, chão, etc.)
    bool onGround = false; // Variável para verificar se o jogador está no chão
    for (const auto& platform : mSolidPlatforms) // Verifique plataformas sólidas
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_WIDTH, PLAYER_HEIGHT, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            if (mVel.y > 0) // Colisão enquanto caindo
            {
                mPos.y = platform.getY() - PLAYER_HEIGHT; // Ajusta a posição para estar em cima da plataforma
                mVel.y = 0; // Zera a velocidade vertical
                onGround = true; // O jogador está no chão
                mFalling = false; // Reseta o estado de queda
            }
            else if (mVel.y < 0) // Colisão enquanto subindo
            {
                mPos.y = platform.getY() + platform.getHeight(); // Ajusta a posição para estar abaixo da plataforma
                mVel.y = 0; // Zera a velocidade vertical
            }
        }
    }

    // Checar também plataformas normais
    for (const auto& platform : mPlatforms)
    {
        if (checkCollision(mPos.x, mPos.y, PLAYER_WIDTH, PLAYER_HEIGHT, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            if (mVel.y > 0) // Colisão enquanto caindo
            {
                mPos.y = platform.getY() - PLAYER_HEIGHT; // Ajusta a posição para estar em cima da plataforma
                mVel.y = 0; // Zera a velocidade vertical
                onGround = true; // O jogador está no chão
                mFalling = false; // Reseta o estado de queda
            }
            else if (mVel.y < 0) // Colisão enquanto subindo
            {
                mPos.y = platform.getY() + platform.getHeight(); // Ajusta a posição para estar abaixo da plataforma
                mVel.y = 0; // Zera a velocidade vertical
            }
        }
    }

    // Se não estiver em uma plataforma, está caindo
    if (!onGround)
    {
        mFalling = true; // O jogador está caindo
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

bool Player::checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
