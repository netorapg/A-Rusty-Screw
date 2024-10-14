#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <list>
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../my-lib-master/include/my-lib/math-vector.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"

class Player
{
public:
    Player(float x, float y, std::list<Platform>& platforms, std::list<SolidPlatform>& solidPlatforms, std::list<Wall>& walls, std::list<Crate>& crates, SDL_Renderer* renderer);
    ~Player();  // Adiciona o destrutor para liberar a textura

    void handleEvent(SDL_Event& e);
    void move();
    void render(SDL_Renderer* renderer);
    bool getQuit() const { return mQuit; } // Método para obter o estado de término

private:
    Mylib::Math::Vector2f mVel;
    Mylib::Math::Vector2f mPos;
    bool mFalling;
    bool mAttacking;
    bool mPassingThroughPlatform;
    bool mQuit; // Adicionado membro para controle de término

    std::list<Platform>& mPlatforms;
    std::list<SolidPlatform>& mSolidPlatforms;
    std::list<Wall>& mWalls;
    std::list<Crate>& mCrates;

    Mylib::Math::Vector2f mAttackPos;

    SDL_Texture* mTexture;  // Adiciona a textura do jogador

    SDL_Rect mSpriteClip;
    int mCurrentFrame;
    int mFrameCount;
    float mAnimationTimer;
    float mAnimationSpeed;

    bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
};

#endif // PLAYER_H