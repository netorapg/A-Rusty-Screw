#include "../include/bettlerider/Player.h"
#include <iostream>

namespace BRTC {

Player::Player(Vector position, SDL_Renderer* renderer)
    : DynamicObject(position, Vector(25, 26)),
      mFacingRight(true),
      mSprite(renderer, "../assets/bezourinha_correndo.png") 
{
    // Configuração das animações
    Animation runAnim;
    runAnim.addFrame({{0,   0, 25, 26}, 0.1f, {0, 0}});
    runAnim.addFrame({{25,  0, 25, 26}, 0.1f, {0, 0}});
    runAnim.addFrame({{50, 0, 25, 26}, 0.1f, {0, 0}});
    runAnim.setLoop(true);

    Animation idleAnim;
    idleAnim.addFrame({{0, 0, 25, 26}, 0.2f, {0, 0}});
    idleAnim.setLoop(true);

    mSprite.addAnimation("run", std::move(runAnim));
    mSprite.addAnimation("idle", std::move(idleAnim));
    mSprite.play("idle");
}

Player::~Player() {
    // A destruição da textura é tratada pela classe Sprite
}

void Player::handleEvent(SDL_Event& e) {
    Vector velocity = getVelocity();

    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_d:
                velocity.x = MOVE_SPEED;
                mFacingRight = true;
                break;
                
            case SDLK_a:
                velocity.x = -MOVE_SPEED;
                mFacingRight = false;
                break;
                
            case SDLK_SPACE:
                if(isOnGround()) {
                    velocity.y = JUMP_FORCE;
                }
                break;
        }
    }
    else if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
            case SDLK_a:
            case SDLK_d:
                velocity.x = 0.0f;
                break;
        }
    }

    setVelocity(velocity);
}

void Player::update(float deltaTime) {
    // Atualização da física
    Vector velocity = getVelocity();
    Vector position = getPosition();

    velocity.y += GRAVITY * deltaTime;
    position += velocity * deltaTime;

    setVelocity(velocity);
    setPosition(position);

    // Controle de animações
    if(velocity.x != 0.0f) {
        mSprite.play("run");
    } else {
        mSprite.play("idle");
    }
    
    mSprite.update(deltaTime);
}

void Player::render(SDL_Renderer* renderer, Vector cameraPosition) {
    Vector screenPosition = getPosition() - cameraPosition;
    mSprite.draw(renderer, 
                static_cast<int>(screenPosition.x), 
                static_cast<int>(screenPosition.y), 
                !mFacingRight);
}

} // namespace BRTC