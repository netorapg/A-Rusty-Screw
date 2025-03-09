#ifndef PLAYER_H
#define PLAYER_H

#include "../my-lib-master/include/my-lib/math-vector.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Player
{
public:
  Player( float x, float y, SDL_Renderer *renderer );
  ~Player();   // Adiciona o destrutor para liberar a textura

  void handleEvent( SDL_Event &e );
  void move();
  void render( SDL_Renderer *renderer,
               float         cameraX,
               float         cameraY );   // Adicionando parâmetros da câmera

  bool getQuit() const
  {
    return mQuit;
  }

  bool isPassingThroughPlatform() const
  {
    return mPassingThroughPlatform;
  }

  // Método para obter o estado de término

  float getPosX() const
  {
    return mPos.x;
  }   // Método para obter a posição x

  float getPosY() const
  {
    return mPos.y;
  }   // Método para obter a posição y

  int  getWidth() const;    // Método para obter a largura
  int  getHeight() const;   // Método para obter a altura
  void reset();

  bool isOnGround() const
  {
    return mOnGround;
  }

  bool isAboveCrate() const
  {
    return mAboveCrate;
  }

  void setOnGround( bool state )
  {
    mOnGround = state;
  }

  void setAboveCrate( bool state )
  {
    mAboveCrate = state;
  }

  void setVerticalVelocity( float vy )
  {
    mVel.y = vy;
  }

  void setHorizontalVelocity( float vx )
  {
    mVel.x = vx;
  }

  float getHorizontalVelocity()
  {
    return mVel.x;
  }

  float getVerticalVelocity()
  {
    return mVel.y;
  }

  void setPosition( float x, float y )
  {
    mPos.x = x;
    mPos.y = y;
  }

  void setFalling( bool falling )
  {
    mFalling = falling;
  }

private:
  Mylib::Math::Vector2f mVel;
  Mylib::Math::Vector2f mPos;
  bool                  mFalling;
  bool                  mAttacking;
  bool                  mPassingThroughPlatform;
  bool                  mQuit;   // Adicionado membro para controle de término
  bool mFacingRight;             // Adicionado membro para controle da direção
  bool mOnGround = false;   // Variável para verificar se o jogador está no chão
  bool mAboveCrate = false;

  Mylib::Math::Vector2f mAttackPos;

  SDL_Texture *mTexture;   // Adiciona a textura do jogador

  SDL_Rect mSpriteClip;
  int      mCurrentFrame;
  int      mFrameCount;
  float    mAnimationTimer;
  float    mAnimationSpeed;

  bool checkCollision( float x1,
                       float y1,
                       float w1,
                       float h1,
                       float x2,
                       float y2,
                       float w2,
                       float h2 );
};

#endif   // PLAYER_H
