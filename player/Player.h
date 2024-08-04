#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <list>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../my-lib-master/include/my-lib/math-vector.h"
#include "../wall/Wall.h"


class Player
{
public:
    Player(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls);

    void handleEvent(SDL_Event& e);
    void move();
    void render(SDL_Renderer* renderer);

private:
    Mylib::Math::Vector2f mVel;
    Mylib::Math::Vector2f mPos;
    bool mFalling;
    bool mAttacking;
    bool mPassingThroughPlatform;

    std::list<Platform>& mPlatforms;
    std::list<SolidPlatform>& mSolidPlatforms;
    std::list<Wall>& mWalls;

    Mylib::Math::Vector2f mAttackPos;

    bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
};

#endif // PLAYER_H
