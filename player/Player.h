#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include <list>
#include "../my-lib-master/include/my-lib/math-vector.h"
#include "../Object.h"

class Player
{
public:
    Player(float x, float y, std::list<Platform>& platforms/*, std::vector<SolidPlatform>& solidPlatforms*/);

    void handleEvent(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

private:
    Mylib::Math::Vector2f mVel;
    Mylib::Math::Vector2f mPos;
    bool mFalling;
    bool mAttacking;
    bool mPassingThroughPlatform;
    Mylib::Math::Vector2f mAttackPos;
    std::list<Platform>& mPlatforms;
    /*std::vector<SolidPlatform> mSolidPlatforms;*/

    bool checkCollision(const SDL_Rect &a, const SDL_Rect &b);
};

#endif // PLAYER_H
