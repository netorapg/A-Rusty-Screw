#include "../include/bettlerider/Crate.h"
namespace BRTC
{

Crate::Crate(float x, float y, float width, float height) 
: Scenario(x, y, width, height), mVel(0, 0) {}

void Crate::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
SDL_Rect fillRect = {
    static_cast<int>(mPosX - cameraX),
    static_cast<int>(mPosY - cameraY),
    static_cast<int>(mWidth),
    static_cast<int>(mHeight)
};
SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
SDL_RenderFillRect(renderer, &fillRect);
}

void Crate::applyForce(float fx, float fy) {
mVel.x += fx;
mVel.y += fy;
}

void Crate::update(const std::list<SolidPlatform>& SolidPlatforms,
              const std::list<Wall>& Walls,
              const std::list<Platform>& Platforms) {
// Aplicar física
mVel.y += mGravity;

// Atualizar posição
mPosX += mVel.x;
mPosY += mVel.y;

// Resolver colisões
handleWallCollisions(Walls);
handlePlatformCollisions(SolidPlatforms, Platforms);

// Aplicar atrito
mVel.x *= mFriction;
if(fabs(mVel.x) < 0.1f) mVel.x = 0;
}

void Crate::handleWallCollisions(const std::list<Wall>& Walls) {
for(const auto& wall : Walls) {
    if(checkCollision(wall.getX(), wall.getY(), 
                     wall.getWidth(), wall.getHeight())) {
        // Colisão horizontal
        if(mVel.x != 0) {
            mPosX -= mVel.x;
            mVel.x = 0;
        }
        // Colisão vertical
        if(mVel.y != 0) {
            mPosY -= mVel.y;
            mVel.y = 0;
        }
    }
}
}

void Crate::handlePlatformCollisions(const std::list<SolidPlatform>& SolidPlatforms,
                                const std::list<Platform>& Platforms) {
// Colisão com plataformas sólidas
for(const auto& platform : SolidPlatforms) {
    if(checkCollision(platform.getX(), platform.getY(),
                     platform.getWidth(), platform.getHeight())) {
        if(mVel.y > 0) { // Caindo
            mPosY = platform.getY() - mHeight;
            mVel.y = 0;
        }
    }
}

// Colisão com plataformas normais
for(const auto& platform : Platforms) {
    if(checkCollision(platform.getX(), platform.getY(),
                     platform.getWidth(), platform.getHeight())) {
        if(mVel.y > 0) { // Caindo
            mPosY = platform.getY() - mHeight;
            mVel.y = 0;
        }
    }
}
}

bool Crate::checkCollision(float x, float y, float w, float h) const {
return (mPosX < x + w &&
        mPosX + mWidth > x &&
        mPosY < y + h &&
        mPosY + mHeight > y);
}

bool Crate::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) {
return (mPosX + mWidth > cameraX && 
        mPosX < cameraX + screenWidth &&
        mPosY + mHeight > cameraY && 
        mPosY < cameraY + screenHeight);
}
}