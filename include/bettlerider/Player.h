#ifndef PLAYER_H
#define PLAYER_H

#include "../../entity/Entity.h"
#include <SDL2/SDL.h>

namespace BRTC
{

class Player : public Entity {
    public:
        Player(float x, float y, SDL_Renderer* renderer);
        ~Player();
    
        void move();
        void update() override;
        void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
        void handleEvent(SDL_Event& e) override;
        int getWidth() const override;   // Sobrescreve o método da Entity
        int getHeight() const override;
        void reset();
        bool isFacingRight() const { return mFacingRight; }
    
    private:
        SDL_Texture* mTexture;
        SDL_Rect mSpriteClip;
        int mCurrentFrame;
        int mFrameCount;
        float mAnimationTimer;
        float mAnimationSpeed;
        bool mFacingRight;
    };

} // namespace BRTC
#endif // PLAYER_H