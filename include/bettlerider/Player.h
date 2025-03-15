#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include <SDL2/SDL.h>

namespace BRTC
{

class Player : public DynamicObject {
    public:
        Player(float x, float y, SDL_Renderer* renderer);
        ~Player();
    
        void update() override;
        void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
        void handleEvent(SDL_Event& e) override;
        void reset();
        bool isFacingRight() const { return mFacingRight; }
        void setPassingThroughPlatform(bool state) { mPassingThroughPlatform = state; }
        void setFalling(bool state) { mFalling = state; }

        int getWidth() const { return mSpriteClip.w; }
        int getHeight() const { return mSpriteClip.h; }
    
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