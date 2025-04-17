#include "../include/bettlerider/Player.h"

namespace BRTC
{

  Player::Player( Vector position, SDL_Renderer *renderer )
  : DynamicObject
    ( 
      position, 
      Vector( 28, 42) 
    ), 
    mFacingDirection(1),
    mIsJumping( false )
  {
    
    SDL_Surface* surface = IMG_Load("../assets/bezourinha_sprites.png");
    if (!surface) 
    { throw std::runtime_error("Failed to load sprite sheet: " + std::string(IMG_GetError())); }
    spriteSheetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!spriteSheetTexture) 
    { throw std::runtime_error("Failed to create texture form sprite sheet: " + std::string(IMG_GetError()));}
  
    Animation runAnim;
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 113, 2, 20, 41 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 146, 2, 32, 41 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 181, 5, 36, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 1, 49, 32, 41 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 113, 2, 20, 41 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 46, 50, 21, 41 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 79, 53, 32, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 46, 50, 21, 41 }), 0.1f, { 0, 0 } } );
      runAnim.setLoop( true );

    Animation idleAnim;
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 6, 1, 28, 42 }), 0.2f, { 0, 0 } } );
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 39, 1, 28, 42 }), 0.2f, { 0, 0 }}  );
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 76, 2, 29, 41 }), 0.2f, { 0, 0 }}  );
      idleAnim.setLoop( true );

    Animation punchAnim;
      punchAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 11, 108, 33, 42 }), 0.1f, { 0, 0 } } );
      punchAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 53, 108, 37, 42 }), 0.1f, { 0, 0 } } );
      punchAnim.setLoop( false );

    Animation strongPunchAnim;
      strongPunchAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 95, 107, 39, 42 }), 0.1f, { 0, 0 } } );
      strongPunchAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 141, 107, 40, 42 }), 0.1f, { 0, 0 } } );
      strongPunchAnim.setLoop( false );

    Animation jumpAnim;
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 120, 50, 31, 49 }), 0.1f, { 0, 0 } } );
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 158, 50, 31, 49 }), 0.1f, { 0, 0 } } );
      jumpAnim.setLoop( true );

    animations["run"] = runAnim;
    animations["idle"] = idleAnim;
    animations["jump"] = jumpAnim;
    animations["punch"] = punchAnim;
    animations["strongPunch"] = strongPunchAnim;
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
        velocity.x   = MOVE_SPEED;
        mFacingDirection = 1;
        break;
      case SDLK_a:
        velocity.x   = -MOVE_SPEED;
        mFacingDirection = -1;
        break;
      case SDLK_SPACE:
        if(isOnGround())
        {
          setOnGround( false );
          velocity.y = JUMP_FORCE;
        }
        if(isCollidingWithWall() && mFacingDirection == 1 )
        {
          setIsCollidingWithWall( false );
          mFacingDirection = -1;
          velocity.x = -MOVE_SPEED;
          velocity.y = JUMP_FORCE;
        }
        if(isCollidingWithWall() && mFacingDirection == -1 )
        {
          setIsCollidingWithWall( false );
          mFacingDirection = 1;
          velocity.x = MOVE_SPEED;
          velocity.y = JUMP_FORCE;
        }
      break;
      case SDLK_s:
        if( isOnGround() ) {
          setPassingThroughPlatform( true );
          setOnGround( false );
        }
        break;
      case SDLK_j:
        mIspunching = true;
        break;
      case SDLK_k:
        mIspunchingHarder = true;
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
      case SDLK_j:
        mIspunching = false;
        break;
      case SDLK_k:
        mIspunchingHarder = false;
        break;   
    }
  }
  setVelocity( velocity );
}
void Player::update(float deltaTime) 
{
  
    Vector velocity = getVelocity();
    Vector position = getPosition();
    velocity.y += GRAVITY * deltaTime;
    position += velocity * deltaTime;
    setVelocity(velocity);
    setPosition(position);


    std::string newAnimation;
    
    
    if (isOnGround()) {
        mIsJumping = false;
        
        if (mIspunching) {
            newAnimation = "punch";
        } 
        else if (mIspunchingHarder) {
            newAnimation = "strongPunch";
        } 
        else if (velocity.x != 0.0f && velocity.y == 0.0f) {
            newAnimation = "run";
        }
        else if (velocity.x != 0.0f) {
            newAnimation = "run";
        } 
        else {
            newAnimation = "idle";
        }
    } 
    else {
        mIsJumping = true;
        newAnimation = "jump";
    }

    // Transição suave entre animações
    if (newAnimation != currentAnimation) {
        animations[newAnimation].reset();
        currentAnimation = newAnimation;
    }

    // Atualiza a animação atual
    animations[currentAnimation].update(deltaTime);
}

void Player::DrawDebugRect
  (
    SDL_Renderer* renderer, 
    int x, 
    int y, 
    int w, 
    int h, 
    Uint8 r, 
    Uint8 g, 
    Uint8 b
  ) 
  {
  SDL_Rect rect = {x, y, w, h};
  SDL_SetRenderDrawColor(renderer, r, g, b, 128);
  SDL_RenderFillRect(renderer, &rect);
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  SDL_RenderDrawRect(renderer, &rect);
  }

  void Player::render(SDL_Renderer* renderer, Vector cameraPosition) 
{
    Vector screenPos = getPosition() - cameraPosition;
    SpritePtr currentSprite = animations[currentAnimation].getCurrentSprite();
    
    if(currentSprite) 
    {
        SDL_Point baseOffset = *animations[currentAnimation].getCurrentOffset();
        const SDL_Rect& frameRect = currentSprite->getSrcRect();
        float baseWidth = 28;
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
            DrawDebugRect
            (
                renderer, 
                static_cast<int>(screenPos.x), 
                static_cast<int>(screenPos.y), 
                getWidth(), 
                getHeight(), 
                255, 0, 0
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
