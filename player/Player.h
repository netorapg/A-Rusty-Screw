#ifndef PLAYER_H
#define PLAYER_H

#include "../entity/Entity.h"
#include <SDL2/SDL.h>

class Player : public Entity {
public:
    Player(float x, float y, SDL_Renderer* renderer);
    ~Player();

    // Implementação dos métodos virtuais
    void update() override;
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
    void handleEvent(SDL_Event& e) override;

    // Métodos específicos do Player
    void reset();
    void move();
    int getWidth() const;
    int getHeight() const;
    bool isFacingRight() const { return mFacingRight; }
    bool isPassingThroughPlatform() const { return mPassingThroughPlatform; }

private:
    SDL_Texture* mTexture;
    SDL_Rect mSpriteClip;
    int mCurrentFrame;
    int mFrameCount;
    float mAnimationTimer;
    float mAnimationSpeed;
    bool mFacingRight;
    bool mPassingThroughPlatform;
    bool mFalling;
};

#endif // PLAYER_H