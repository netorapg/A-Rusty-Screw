// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "../my-lib-master/include/my-lib/math-vector.h"
#include "../entity/Entity.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <list>

const int   PLAYER_WIDTH  = 25;
const int   PLAYER_HEIGHT = 26;

class Player : public Entity
{
public:
  // Construtor agora aceita um SDL_Renderer*
  Player( float x, float y, SDL_Renderer *renderer );

  ~Player();  // Destruidor

  void handleEvent( SDL_Event &e );
  void move( std::list<Platform>      &platforms,
             std::list<SolidPlatform> &solidPlatforms,
             std::list<Wall>          &walls,
             std::list<Crate>         &crates ) override;
  void render( SDL_Renderer *renderer, float cameraX, float cameraY );
  void reset();

  bool getQuit() const
  {
    return mQuit;
  }

  float getPosX() const
  {
    return mPos.x;
  }

  float getPosY() const
  {
    return mPos.y;
  }

  int getWidth() const;
  int getHeight() const;

private:
  Mylib::Math::Vector2f mVel;
  Mylib::Math::Vector2f mPos;
  bool                  mFalling;
  bool                  mAttacking;
  bool                  mPassingThroughPlatform;
  bool                  mQuit;
  bool                  mFacingRight;
  bool                  mOnGround;
  bool                  mAboveCrate;

  SDL_Texture *mTexture;
  SDL_Rect     mSpriteClip;
  int          mCurrentFrame;
  int          mFrameCount;
  float        mAnimationTimer;
  float        mAnimationSpeed;
};

#endif   // PLAYER_H
