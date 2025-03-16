#include "../include/bettlerider/Player.h"
#include <iostream>

namespace BRTC {

Player::Player(Vector position, SDL_Renderer* renderer)
    : DynamicObject(position, Vector(40, 48)),
      mFacingRight(true),
      mSprite(renderer, "../assets/bezourinha_sprites.png") 
{
    // Configuração das animações
    Animation runAnim;
    runAnim.addFrame({{0, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{45, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{92, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{138, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{0, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{184, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{230, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.addFrame({{276, 204, 50, 48}, 5.5f, {0, 0}});
    runAnim.setLoop(true);

    Animation idleAnim;
    idleAnim.addFrame({{0, 0, 40, 48}, 7.5f, {0, 0}});
    idleAnim.addFrame({{45, 0, 40, 48}, 7.5f, {0, 0}});
    idleAnim.addFrame({{91, 0, 40, 48}, 7.5f, {0, 0}});
    idleAnim.setLoop(true);

    Animation punchAnim;
    punchAnim.addFrame({{0, 102, 29, 45}, 0.1f, {0, 0}});
    punchAnim.addFrame({{46, 102, 29, 45}, 0.1f, {0, 0}});
    punchAnim.addFrame({{0, 102, 29, 45}, 0.1f, {0, 0}});
    punchAnim.setLoop(false);

    Animation strongPunchAnim;
    strongPunchAnim.addFrame({{0, 153, 29, 45}, 0.1f, {0, 0}});
    strongPunchAnim.addFrame({{46, 153, 29, 45}, 0.1f, {0, 0}});
    strongPunchAnim.setLoop(false);

    Animation jumpAnim;
    jumpAnim.addFrame({{0, 50, 29, 45}, 0.1f, {0, 0}});
    jumpAnim.addFrame({{46, 51, 29, 45}, 0.1f, {0, 0}});
    jumpAnim.setLoop(false);


    mSprite.addAnimation("run", std::move(runAnim));
    mSprite.addAnimation("idle", std::move(idleAnim));
    mSprite.addAnimation("punch", std::move(punchAnim));
    mSprite.addAnimation("strong_punch", std::move(strongPunchAnim));
    mSprite.addAnimation("jump", std::move(jumpAnim));
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
    }  
    else if(!isOnGround()) {
        mSprite.play("jump");
    } 
    else {
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