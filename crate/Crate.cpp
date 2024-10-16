#include "Crate.h"

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

void Crate::update(const std::list<SolidPlatform>& SolidPlatforms, const std::list<Wall>& Walls, const std::list<Platform>& Platforms) {
    // Se não está colidindo com plataformas, aplica gravidade
    if (!isCollidingWithPlatforms(SolidPlatforms, Platforms)) {
        GRAVITY += 0.1;
        mPosY += GRAVITY;
    } else {
        GRAVITY = 0.5f;
    }

    // Checa colisão com paredes e ajusta a posição do caixote
    for (const auto& Wall : Walls) {
        // Colisão à direita
        if (checkCollision(mPosX + CRATE_SIZE, mPosY, 1, CRATE_SIZE, Wall.getX(), Wall.getY(), Wall.getWidth(), Wall.getHeight())) {
            mPosX = Wall.getX() - CRATE_SIZE; // Ajusta para ficar exatamente ao lado da parede
        }
        // Colisão à esquerda
        else if (checkCollision(mPosX - 1, mPosY, 1, CRATE_SIZE, Wall.getX(), Wall.getY(), Wall.getWidth(), Wall.getHeight())) {
            mPosX = Wall.getX() + Wall.getWidth(); // Ajusta para ficar exatamente ao lado da parede
        }
    }
}

bool Crate::isCollidingWithPlatforms(const std::list<SolidPlatform>& SolidPlatforms, const std::list<Platform>& Platforms) {
    for (const auto& SolidPlatform : SolidPlatforms) {
        if (checkCollision(mPosX, mPosY + CRATE_SIZE, CRATE_SIZE, 1, SolidPlatform.getX(), SolidPlatform.getY(), SolidPlatform.getWidth(), SolidPlatform.getHeight())) {
            mPosY = SolidPlatform.getY() - CRATE_SIZE; // Ajusta a posição do caixote para ficar em cima da plataforma
            return true; // Está colidindo
        }
    }
    for (const auto& Platform : Platforms) {
        if (checkCollision(mPosX, mPosY + CRATE_SIZE, CRATE_SIZE, 1, Platform.getX(), Platform.getY(), Platform.getWidth(), Platform.getHeight())) {
            mPosY = Platform.getY() - CRATE_SIZE; // Ajusta a posição do caixote para ficar em cima da plataforma
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
