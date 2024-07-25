#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Platform.h"
#include <vector>
#include "my-lib-master/include/my-lib/math-vector.h"

class Player
{
public:
    Player(float x, float y, std::vector<Platform>& platforms);

    void handleEvent(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

private:
    Mylib::Math::Vector2f mVel;
    Mylib::Math::Vector2f mPos;
    bool mFalling;
    bool mAttacking;
    bool mPassingThroughPlatform;
    Mylib::Math::Vector2f mAttackPos;
    std::vector<Platform>& mPlatforms;

    bool checkCollision(const SDL_Rect &a, const SDL_Rect &b);
};

#endif // PLAYER_H