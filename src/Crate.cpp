#include "../include/arscrew/Crate.h"

namespace ARSCREW
{
    Crate::Crate
    (
        Vector position, 
        SDL_Renderer* renderer
    ) : DynamicObject(position, Vector(80, 80)) 
    {
        setVelocity(Vector::zero());
    }
    void Crate::render
    (
        SDL_Renderer* renderer,
        Vector cameraPosition
    ) 
    {
        Vector screenPosition = getPosition() - cameraPosition;
        SDL_Rect fillRect = 
        {
            static_cast<int>(screenPosition.x),
            static_cast<int>(screenPosition.y),
            static_cast<int>(getSize().x),
            static_cast<int>(getSize().y)
        };
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_RenderFillRect(renderer, &fillRect);
    }
    void Crate::applyForce(Vector force) 
    {
        setVelocity(getVelocity() + force);
    }
    void Crate::update(float deltaTime) 
    {
        Vector velocity = getVelocity();
        Vector position = getPosition();   
        velocity.y += ARSCREW::GRAVITY * deltaTime;
        velocity.x *= ARSCREW::FRICTION;
        position += velocity * deltaTime;
        setVelocity(velocity);
        setPosition(position);
    }
}