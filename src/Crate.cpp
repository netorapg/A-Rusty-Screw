#include "../include/bettlerider/Crate.h"

namespace BRTC
{

Crate::Crate(float x, float y, float width, float height) 
: DynamicObject(x, y, width, height) // Chama o construtor de DynamicObject
{
    setVelocity(0, 0); // Inicializa velocidade
}

void Crate::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    SDL_Rect fillRect = {
        static_cast<int>(getX() - cameraX), // Usa getX() de Object
        static_cast<int>(getY() - cameraY), // Usa getY() de Object
        static_cast<int>(getWidth()),
        static_cast<int>(getHeight())
    };
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_RenderFillRect(renderer, &fillRect);
}

void Crate::applyForce(float fx, float fy) {
    setHorizontalVelocity(getHorizontalVelocity() + fx);
    setVerticalVelocity(getVerticalVelocity() + fy);
}

void Crate::update() {
    // Aplicar física usando métodos de DynamicObject
    setVerticalVelocity(getVerticalVelocity() + mGravity);
    setHorizontalVelocity(getHorizontalVelocity() * mFriction);
    if (std::abs(getHorizontalVelocity()) < 0.1f) {
        setHorizontalVelocity(0.0f);
    }
    setPosition(getX() + getHorizontalVelocity(), getY() + getVerticalVelocity());
}

bool Crate::checkCollision(float x, float y, float w, float h) const {
    return (getX() < x + w &&
            getX() + getWidth() > x &&
            getY() < y + h &&
            getY() + getHeight() > y);
}



// Implemente handleWallCollisions e handlePlatformCollisions usando getters
void Crate::handleWallCollisions(const std::list<Wall>& Walls) {
    for (const auto& wall : Walls) {
        if (checkCollision(wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight())) {
            // Lógica de colisão atualizada
            if (getHorizontalVelocity() != 0) {
                setPosition(getX() - getHorizontalVelocity(), getY());
                setHorizontalVelocity(0);
            }
            if (getVerticalVelocity() != 0) {
                setPosition(getX(), getY() - getVerticalVelocity());
                setVerticalVelocity(0);
            }
        }
    }
}

// Implemente handlePlatformCollisions de forma similar...

} // namespace BRTC