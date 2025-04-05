#include "../include/bettlerider/Player.h"

#include <iostream>

namespace BRTC
{

Player::Player( Vector position, SDL_Renderer *renderer )
    : DynamicObject( position, Vector( 29, 29) ), mFacingRight( true )
{

  SDL_Surface* surface = IMG_Load("../assets/bezourinha_sprites.png");
  if (!surface) {
    throw std::runtime_error("Failed to load sprite sheet: " + std::string(IMG_GetError()));
  }
  SDL_Texture* spriteSheetTexture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  if (!spriteSheetTexture) {
    throw std::runtime_error("Failed to create texture form sprite sheet: " + std::string(IMG_GetError()));
  }
  

  // Configuração das animações
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

//Player::~Player(){}

void Player::handleEvent( SDL_Event &e )
{
  Vector velocity = getVelocity();

  if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
  {
    switch( e.key.keysym.sym )
    {
      case SDLK_d:
        velocity.x   = MOVE_SPEED;
        mFacingRight = true;
        break;

      case SDLK_a:
        velocity.x   = -MOVE_SPEED;
        mFacingRight = false;
        break;

      case SDLK_SPACE:
        if(isOnGround() )
        {
          setOnGround( false );
          mIsJumping = true;
          velocity.y = JUMP_FORCE;
        }
        break;
      case SDLK_s:
        if( isOnGround() )
          setPassingThroughPlatform( true );
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
  // Atualização da física
  Vector velocity = getVelocity();
  Vector position = getPosition();

  velocity.y += GRAVITY * deltaTime;
  position += velocity * deltaTime;

  setVelocity( velocity );
  setPosition( position );

  std::string previousAnimation = currentAnimation; 

  if( isOnGround() ) 
    mIsJumping = false;
  if( isOnGround() && velocity.x != 0.0f )
    currentAnimation = "run";
  else if( mIsJumping )
    currentAnimation = "jump";
  else if( mIspunching )
    currentAnimation = "punch";
  else if( mIspunchingHarder )
    currentAnimation = "strongPunch";
  else
    currentAnimation = "idle";
  if (previousAnimation != currentAnimation) {
    animations[currentAnimation].reset();
  }
  animations[currentAnimation].update(deltaTime);
}

void Player::render( SDL_Renderer *renderer, Vector cameraPosition )
{
  Vector screenPos = getPosition() - cameraPosition;
  SpritePtr currentSprite = animations[currentAnimation].getCurrentSprite();
  if(currentSprite) {
    SDL_Point offset = *animations[currentAnimation].getCurrentOffset();
    currentSprite->draw(renderer, static_cast<int>(screenPos.x + offset.x), static_cast<int>(screenPos.y + offset.y), !mFacingRight);
  }
}

void Player::setPassingThroughPlatform( bool enable )
{
  mPassingThroughPlatform = enable;
}

}   // namespace BRTC
