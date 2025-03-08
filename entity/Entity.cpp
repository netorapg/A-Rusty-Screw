// Entity.cpp
#include "Entity.h"

void Entity::move(std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, 
                  std::list<Wall>& walls, std::list<Crate>& crates)
{
    // Gravidade
    mVelY += GRAVITY;

    // Atualizar posição horizontal
    mPosX += mVelX;

    // Verificar colisão horizontal com paredes
    for (const auto& wall : walls)
    {
        if (checkCollision(mPosX, mPosY, mWidth, mHeight, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
        {
            if (mVelX > 0) { mPosX = wall.getX() - mWidth; }
            else if (mVelX < 0) { mPosX = wall.getX() + wall.getWidth(); }
            mVelX = 0;
        }
    }

    // Atualizar posição vertical
    mPosY += mVelY;

    // Verificar colisão vertical com plataformas sólidas
    for (const auto& platform : solidPlatforms)
    {
        if (checkCollision(mPosX, mPosY, mWidth, mHeight, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
        {
            if (mVelY > 0)  // Caindo
            {
                mPosY = platform.getY() - mHeight;
                mVelY = 0;
                mOnGround = true;
                mFalling = false;
            }
            else if (mVelY < 0)  // Subindo
            {
                mPosY = platform.getY() + platform.getHeight();
                mVelY = 0;
            }
        }
    }

    // Verificar colisão com plataformas normais
    for (const auto& platform : platforms)
    {
        if (checkCollision(mPosX, mPosY + mHeight, mWidth, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()) && mVelY >= 0)
        {
            mPosY = platform.getY() - mHeight;
            mVelY = 0;
            mOnGround = true;
            mFalling = false;
            break;
        }
    }

    if (!mOnGround)
    {
        mFalling = true;
    }

    // Verificar colisão com caixas
    for (auto& crate : crates)
    {
        if (checkCollision(mPosX, mPosY, mWidth, mHeight, crate.getX(), crate.getY(), crate.getWidth(), crate.getHeight()))
        {
            // Lógica de colisão com caixa, semelhante ao caso do Player.
        }
    }
}

bool Entity::checkCollision(float x1, float y1, float w1, float h1, 
                            float x2, float y2, float w2, float h2)
{
    // Verifica se há interseção entre os dois retângulos
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
