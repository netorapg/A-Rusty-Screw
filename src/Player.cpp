#include "../include/bettlerider/Player.h"

#include <iostream>

namespace BRTC
{

Player::Player( Vector position, SDL_Renderer *renderer )
    : DynamicObject( position, Vector( 29, 29) ), mFacingRight( true ),
      mSprite( renderer, "../assets/bezourinha_sprites.png" )
{
  // Configuração das animações
  Animation runAnim;
  runAnim.addFrame( { { 113, 2, 20, 41 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 146, 2, 32, 41 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 181, 5, 36, 37 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 1, 49, 32, 41 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 113, 2, 20, 41 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 46, 50, 21, 41 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 79, 53, 32, 37 }, 0.1f, { 0, 0 } } );
  runAnim.addFrame( { { 46, 50, 21, 41 }, 0.1f, { 0, 0 } } );
  runAnim.setLoop( true );

  Animation idleAnim;
  idleAnim.addFrame( { { 6, 1, 28, 42 }, 0.2f, { 0, 0 } } );
  idleAnim.addFrame( { { 39, 1, 28, 42 }, 0.2f, { 0, 0 } } );
  idleAnim.addFrame( { { 76, 2, 29, 41 }, 0.2f, { 0, 0 } } );
  idleAnim.setLoop( true );

  Animation punchAnim;
  punchAnim.addFrame( { { 11, 108, 33, 42 }, 0.1f, { 0, 0 } } );
  punchAnim.addFrame( { { 53, 108, 37, 42 }, 0.1f, { 0, 0 } } );
  punchAnim.setLoop( false );

  Animation strongPunchAnim;
  strongPunchAnim.addFrame( { { 95, 107, 39, 42 }, 0.1f, { 0, 0 } } );
  strongPunchAnim.addFrame( { { 141, 107, 40, 42 }, 0.1f, { 0, 0 } } );
  strongPunchAnim.setLoop( false );

  Animation jumpAnim;
  jumpAnim.addFrame( { { 120, 50, 31, 49 }, 0.1f, { 0, 0 } } );
  jumpAnim.addFrame( { { 158, 50, 31, 49 }, 0.1f, { 0, 0 } } );
  jumpAnim.setLoop( true );

  mSprite.addAnimation( "run", std::move( runAnim ) );
  mSprite.addAnimation( "idle", std::move( idleAnim ) );
  mSprite.addAnimation( "punch", std::move( punchAnim ) );
  mSprite.addAnimation( "strong_punch", std::move( strongPunchAnim ) );
  mSprite.addAnimation( "jump", std::move( jumpAnim ) );
  mSprite.play( "idle" );
}

Player::~Player(){}

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

void Player::update()
{
  // Atualização da física
  Vector velocity = getVelocity();
  Vector position = getPosition();

  velocity.y += GRAVITY * deltaTime;
  position += velocity * deltaTime;

  setVelocity( velocity );
  setPosition( position );

  if( isOnGround() ) 
    mIsJumping = false;
  if( isOnGround() && velocity.x != 0.0f )
    mSprite.play( "run" );
  else if( mIsJumping )
    mSprite.play( "jump" );
  else if( mIspunching )
    mSprite.play( "punch" );
  else if( mIspunchingHarder )
    mSprite.play( "strong_punch" );
  else
    mSprite.play( "idle" );

  mSprite.update(deltaTime);
}

void Player::render( SDL_Renderer *renderer, Vector cameraPosition )
{
  Vector screenPosition = getPosition() - cameraPosition;
  mSprite.draw( renderer,
                static_cast<int>( screenPosition.x ),
                static_cast<int>( screenPosition.y ),
                !mFacingRight );
}

void Player::setPassingThroughPlatform( bool enable )
{
  mPassingThroughPlatform = enable;
}

}   // namespace BRTC
