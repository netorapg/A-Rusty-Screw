// Crate.cpp
#include "Crate.h"
#include "../platforms/SolidPlatform.h"

float GRAVITY = 0.1f; // Força da gravidade
const float CRATE_SIZE = 50;
bool falling = false;

Crate::Crate(float x, float y, float width, float height) : Object(x, y, width, height) {}

void Crate::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    // Calcule a posição relativa à câmera
    SDL_Rect fillRect = {
        static_cast<int>(mPosX - cameraX),
        static_cast<int>(mPosY - cameraY),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight)
    };
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Cor do caixote
    SDL_RenderFillRect(renderer, &fillRect);
}

void Crate::update(const std::list<SolidPlatform>& SolidPlatforms) {
    // Se não está colidindo com plataformas, aplica gravidade
    if (!isCollidingWithPlatforms(SolidPlatforms)) {
        GRAVITY += 0.1;
        mPosY += GRAVITY;
    } else {
        GRAVITY = 0.5f;
    }
}

bool Crate::isCollidingWithPlatforms(const std::list<SolidPlatform>& SolidPlatforms) {
    for (const auto& SolidPlatform : SolidPlatforms) {
        if (checkCollision(mPosX, mPosY + CRATE_SIZE, CRATE_SIZE, 1, SolidPlatform.getX(), SolidPlatform.getY(), SolidPlatform.getWidth(), SolidPlatform.getHeight())) {
            mPosY = SolidPlatform.getY() - CRATE_SIZE; // Ajusta a posição do caixote para ficar em cima da plataforma
            return true; // Está colidindo
        }
    }
    return false; // Não está colidindo com nenhuma plataforma
}

bool Crate::isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) {
    // Verifica se a plataforma está visível na tela
    return (mPosX + mWidth > cameraX && mPosX < cameraX + screenWidth &&
            mPosY + mHeight > cameraY && mPosY < cameraY + screenHeight);
}