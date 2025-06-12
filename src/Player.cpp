#include "../include/arscrew/Player.h"

namespace ARSCREW
{

  Player::Player( Vector position, SDL_Renderer *renderer )
  : DynamicObject
    ( 
      position, 
      Vector( 20, 37) 
    ), 
    mFacingDirection(1),
    mIsJumping( false )
  {
    
    SDL_Surface* surface = IMG_Load("../assets/screwer_sprites.png");
    if (!surface) 
    { throw std::runtime_error("Failed to load sprite sheet: " + std::string(IMG_GetError())); }
    spriteSheetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!spriteSheetTexture) 
    { throw std::runtime_error("Failed to create texture form sprite sheet: " + std::string(IMG_GetError()));}
  
    Animation runAnim;
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 105, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 175, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.setLoop( true );

    Animation idleAnim;
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 0, 35, 37 }), 0.2f, { 0, 0 } } );
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 0, 35, 37 }), 0.2f, { 0, 0 }}  );
      idleAnim.setLoop( true );

    
    Animation cuttingAttackAnim;
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 37, 35, 37 }), 0.09f, { 0, 0 } } );
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 178, 53, 40, 27 }), 0.01f, { 0, 10 } } );
      cuttingAttackAnim.setLoop( false );

    
    Animation piercingAttackAnim;
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 74, 36, 37 }), 0.1f, { 0, 0 } } );
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 270, 84, 56, 27 }), 0.1f, { 0, 10 } } );
      piercingAttackAnim.setLoop( false );

    Animation jumpAnim;
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      jumpAnim.setLoop( true );

    animations["run"] = runAnim;
    animations["idle"] = idleAnim;
    animations["jump"] = jumpAnim;
    animations["cuttingAttack"] = cuttingAttackAnim;
    animations["piercingAttack"] = piercingAttackAnim;
    currentAnimation = "idle";
}

void Player::handleEvent( SDL_Event &e )
{
  Vector velocity = getVelocity();
  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    switch( e.key.keysym.sym )
    {
      case SDLK_d:
        velocity.x = MOVE_SPEED;
        mFacingDirection = 1;
        break;
      case SDLK_a:
        velocity.x = -MOVE_SPEED;
        mFacingDirection = -1;
        break;
      case SDLK_SPACE:
        if(isOnGround())
        {
          setOnGround( false );
          velocity.y = JUMP_FORCE;
        }
        handleWallJump(velocity);
      break;
      case SDLK_s:
        if( isOnGround() ) {
          setPassingThroughPlatform( true );
          setOnGround( false );
        }
        break;
      case SDLK_j:
        if (!mIsAttacking) {
            mIsAttacking = true;
            mAttackDuration = ATTACK_DURATION;
            float hitboxWidth;
            float hitboxHeight;
            float offsetX;
            float offsetY;
            
            if (mCurrentAttackType == AttackType::CUTTING) {
                hitboxWidth = 20; 
                hitboxHeight = 20; 
                offsetX = 1;
                offsetY = 2;
            } else {
                hitboxWidth = 35; 
                hitboxHeight = 10; 
                offsetX = 1;
                offsetY = 5.5; 
            }

            if (mFacingDirection == 1) {
                mAttackHitbox.x = getPosition().x + getWidth() - offsetX;
            } else {
                mAttackHitbox.x = getPosition().x - hitboxWidth + offsetX; 
            }

            mAttackHitbox.y = getPosition().y + getHeight() / 2 - hitboxHeight / 2 + offsetY;
            mAttackHitbox.w = hitboxWidth;
            mAttackHitbox.h = hitboxHeight;
        }
        break;
      case SDLK_q:
        switchAttackType();
        break;
      case SDLK_e:
        if (!mIsDashing && isOnGround() && !isCollidingWithWall()) {
            mIsDashing = true;
            mDashTimer = DASH_DURATION;
            if (mFacingDirection == 1) {
                velocity.x += DASH_SPEED;
            } 
            else if (mFacingDirection == -1) {
                velocity.x += -DASH_SPEED;
            }
        }
        break;
      case SDLK_LCTRL:
        mShowDebugRects = !mShowDebugRects;
        break;
    }
  }
  else if( e.type == SDL_KEYUP )
  {
    switch( e.key.keysym.sym )
    {
      case SDLK_a:
      case SDLK_d:
        velocity.x = 0.0f;
        break;
      case SDLK_s:
        setPassingThroughPlatform( false );
        break;
    }
  }
  setVelocity( velocity );
}

void Player::switchAttackType()
{
    if (mCurrentAttackType == AttackType::CUTTING) {
        mCurrentAttackType = AttackType::PIERCING;
        std::cout << "Switched to PIERCING attack (for Phillips screws)" << std::endl;
    } else {
        mCurrentAttackType = AttackType::CUTTING;
        std::cout << "Switched to CUTTING attack (for Flathead screws)" << std::endl;
    }
}

void Player::handleWallJump(Vector& velocity)
{
    if (isCollidingWithWall() && mFacingDirection == 1)
    {
        velocity.x = -MOVE_SPEED;
        velocity.y = JUMP_FORCE;
        mFacingDirection = -1;
        setIsCollidingWithWall(false);
    }
    else if (isCollidingWithWall() && mFacingDirection == -1)
    {
        velocity.x = MOVE_SPEED;
        velocity.y = JUMP_FORCE;
        mFacingDirection = 1;
        setIsCollidingWithWall(false);
    }
    setVelocity(velocity);
}

void Player::update(float deltaTime) 
{
    Vector velocity = getVelocity();
    Vector position = getPosition();

    if (mIsDashing) {
      mDashTimer -= deltaTime;
      if (mFacingDirection == 1) {
          velocity.x = 0.0f;
          velocity.x += DASH_SPEED;
          position += velocity * deltaTime;
      } else if (mFacingDirection == -1) {
          velocity.x = 0.0f;
          velocity.x += -DASH_SPEED;
          position += velocity * deltaTime;
      }
      if (mDashTimer <= 0.0f) {
          mIsDashing = false;
          velocity.x = 0.0f;
      }
    }

    velocity.y += GRAVITY * deltaTime;

    position += velocity * deltaTime;

    if (isOnGround()) {
        setIsCollidingWithWall(false);
        mIsJumping = false;
    
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]) {
            velocity.x = 0.0f;
        }
    } else {
        mIsJumping = true;
    }

    std::string newAnimation;
    if (isOnGround()) {
        if (mIsAttacking) {
            if (mCurrentAttackType == AttackType::CUTTING) {
                newAnimation = "cuttingAttack";
            } else {
                newAnimation = "piercingAttack";
            }
        } else if (velocity.x != 0.0f || mIsDashing) {
            newAnimation = "run";
        } else {
            newAnimation = "idle";
        }
    } else {
        newAnimation = "jump";
    }

    if (newAnimation != currentAnimation) {
        animations[newAnimation].reset();
        currentAnimation = newAnimation;
    }

    animations[currentAnimation].update(deltaTime);

    setVelocity(velocity);
    setPosition(position);

    updateHurtbox();

    if (mIsAttacking) {
        mAttackDuration -= deltaTime;
        if (mAttackDuration <= 0.0f) {
            mIsAttacking = false;
        }
    }
}

void Player::updateHurtbox()
{
    Vector position = getPosition();
    
    float playerWidth = getWidth();
    float playerHeight = getHeight();
    
    float hurtboxWidthScale = 0.7f;
    float hurtboxHeightScale = 0.7f;
    
    float hurtboxXOffset = 0.0f;
    float hurtboxYOffset = 5.0f;
    
    float hurtboxWidth = playerWidth * hurtboxWidthScale;
    float hurtboxHeight = playerHeight * hurtboxHeightScale;
    
    float centeringOffsetX = (playerWidth - hurtboxWidth) / 2.0f;
    float centeringOffsetY = (playerHeight - hurtboxHeight) / 2.0f;
    
    mHurtbox.x = position.x + centeringOffsetX + hurtboxXOffset;
    mHurtbox.y = position.y + centeringOffsetY + hurtboxYOffset;
    
    mHurtbox.w = static_cast<int>(hurtboxWidth);
    mHurtbox.h = static_cast<int>(hurtboxHeight);
}


void Player::DrawDebugOutline
(
    SDL_Renderer* renderer, 
    int x, 
    int y, 
    int w, 
    int h, 
    Uint8 r, 
    Uint8 g, 
    Uint8 b,
    int thickness
) 
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    for (int i = 0; i < thickness; i++) {
        SDL_RenderDrawLine(renderer, x - i, y - i, x + w + i, y - i);
        SDL_RenderDrawLine(renderer, x - i, y + h + i, x + w + i, y + h + i);
        SDL_RenderDrawLine(renderer, x - i, y - i, x - i, y + h + i);
        SDL_RenderDrawLine(renderer, x + w + i, y - i, x + w + i, y + h + i);
    }
}

  void Player::updateWeaponPosition() {
   
  }

  void Player::render(SDL_Renderer* renderer, Vector cameraPosition) 
{
    Vector screenPos = getPosition() - cameraPosition;

    if (mShowHurtbox && mShowDebugRects) {
        SDL_Rect hurtboxOnScreen = {
            mHurtbox.x - static_cast<int>(cameraPosition.x),
            mHurtbox.y - static_cast<int>(cameraPosition.y),
            mHurtbox.w,
            mHurtbox.h
        };
        
        DrawDebugOutline(renderer, hurtboxOnScreen.x, hurtboxOnScreen.y, 
                        hurtboxOnScreen.w, hurtboxOnScreen.h, 255, 255, 0, 2);
    }

    if (mIsAttacking && mShowAttackHitbox && mShowDebugRects) {
        SDL_Rect attackRectOnScreen = {
            mAttackHitbox.x - static_cast<int>(cameraPosition.x),
            mAttackHitbox.y - static_cast<int>(cameraPosition.y),
            mAttackHitbox.w,
            mAttackHitbox.h
        };

        if (mCurrentAttackType == AttackType::CUTTING) {
            DrawDebugOutline(renderer, attackRectOnScreen.x, attackRectOnScreen.y, 
                           attackRectOnScreen.w, attackRectOnScreen.h, 0, 255, 0, 2);
        } else {
            DrawDebugOutline(renderer, attackRectOnScreen.x, attackRectOnScreen.y, 
                           attackRectOnScreen.w, attackRectOnScreen.h, 0, 0, 255, 2);
        }
    }


    SpritePtr currentSprite = animations[currentAnimation].getCurrentSprite();
    
    if(currentSprite) 
    {
        SDL_Point baseOffset = *animations[currentAnimation].getCurrentOffset();
        const SDL_Rect& frameRect = currentSprite->getSrcRect();
        float baseWidth = 20;
        float frameWidth = frameRect.w;
        float widthDifference = frameWidth - baseWidth;
        Vector renderOffset;
        renderOffset.x = static_cast<int>(screenPos.x);
        renderOffset.y = static_cast<int>(screenPos.y);
        if(mFacingDirection == -1) 
        { renderOffset.x -= widthDifference * 1.0f; } 
        else 
        { renderOffset.x += widthDifference * 0.0f; }
        renderOffset.x += baseOffset.x;
        renderOffset.y += baseOffset.y;
        
        if (mShowDebugRects) 
        {
            DrawDebugOutline
            (
                renderer, 
                static_cast<int>(screenPos.x), 
                static_cast<int>(screenPos.y), 
                getWidth(), 
                getHeight(), 
                255, 0, 0, 1
            );
        }
        currentSprite->draw(renderer, renderOffset.x, renderOffset.y, mFacingDirection == -1);
    }
}

  void Player::setPassingThroughPlatform( bool enable )
  {
    mPassingThroughPlatform = enable;
  }
}
