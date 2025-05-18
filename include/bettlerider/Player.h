#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>
#include "Object.h"
#include "Animation.h"
#include "Sprite.h"
#include "Globals.h"

namespace BRTC 
{
    class Player : public DynamicObject 
    {
        SDL_Texture* spriteSheetTexture;
        public:
            Player
            (
                Vector position, 
                SDL_Renderer* renderer
            );
            ~Player() 
            {
                animations.clear();
                if (spriteSheetTexture) 
                {
                    SDL_DestroyTexture(spriteSheetTexture);
                }
            }
            void update(float deltaTime) override;
            void render
            (
                SDL_Renderer* renderer, 
                Vector cameraPosition
            ) 
            override;
            void handleWallJump(Vector& velocity);
            void handleEvent(SDL_Event& e) override;
            void setPassingThroughPlatform(bool enable);
            bool isFacingRight() const { return mFacingDirection == 1; }
            int getFacingDirection() const { return mFacingDirection; }
            int getWidth() const { return static_cast<int>(mSize.x); }
            int getHeight() const { return static_cast<int>(mSize.y); }
        private:
           int mFacingDirection;
            bool mIsJumping;
            bool mIspunching = false;
            bool mIspunchingHarder = false;
            bool mIsFalling = false;
            bool mShowDebugRects = true; 
            bool mIsDashing = false;
            float mDashTimer = 0.0f;
            const float DASH_DURATION = 0.2f;
            const float DASH_SPEED = 500.0f;
            std::unordered_map<std::string, Animation> animations;
            std::string currentAnimation;
            Vector mWeaponOffsetRight = {-1, -2};
            //Vector mWeaponOffsetLeft = {-5, 2};
            Vector mPunchOffset;
            Vector mStrongPunchOffset;
            //void mDashSpeed(int direction, float speedInX, int i);
            
            void updateWeaponPosition();

            void DrawDebugRect
            (
                SDL_Renderer* renderer, 
                int x, 
                int y, 
                int w, 
                int h, 
                Uint8 r, 
                Uint8 g, 
                Uint8 b
            );
    };
}
#endif