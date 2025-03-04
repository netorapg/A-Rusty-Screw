#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <list>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"
#include "../my-lib-master/include/my-lib/math-vector.h"
#include "../collision/CollisionHandler.h"

class Player
{
public:
    Player(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls, std::list<Crate>& crates, SDL_Renderer* renderer);
    ~Player();  

    void handleEvent(SDL_Event& e);
    void move();
    void render(SDL_Renderer* renderer, float cameraX, float cameraY); 
    void reset();
    
    bool getQuit() const { return mQuit; }
    float getPosX() const { return mPos.x; }
    float getPosY() const { return mPos.y; }
    int getWidth() const;
    int getHeight() const;

private:
    Mylib::Math::Vector2f mVel;
    Mylib::Math::Vector2f mPos;
    bool mFalling;
    bool mAttacking;
    bool mPassingThroughPlatform;
    bool mQuit;
    bool mFacingRight;
    bool mOnGround;
    bool mAboveCrate;

    std::list<Platform>& mPlatforms;
    std::list<SolidPlatform>& mSolidPlatforms;
    std::list<Wall>& mWalls;
    std::list<Crate>& mCrates;

    SDL_Texture* mTexture;  
    SDL_Rect mSpriteClip;
    int mCurrentFrame;
    int mFrameCount;
    float mAnimationTimer;
    float mAnimationSpeed;
};

#endif // PLAYER_H
