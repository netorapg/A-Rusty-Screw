#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Platform.h"
#include <vector>

class Player
{
public:
    Player(int x, int y, std::vector<Platform>& platforms);

    void handleEvent(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision(std::vector<Platform> &platforms);

private:
    int mVelX, mVelY;
    int mPosX, mPosY;
    bool mFalling;
    bool mAttacking;
    int mAttackPosX, mAttackPosY;
    std::vector<Platform>& mPlatforms;

    bool checkCollision(const SDL_Rect &a, const SDL_Rect &b);
};

#endif // PLAYER_H
