#ifndef CHICKEN_H
#define CHICKEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>
#include "Object.h"
#include "Animation.h"
#include "Sprite.h"
#include "Globals.h"

namespace ARSCREW
{
    class Player;

    class Chicken : public DynamicObject
    {
    public:
        Chicken(Vector position, SDL_Renderer* renderer);
        ~Chicken();

        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        void handleEvent(SDL_Event& e) override;

        void followPlayer(const Player& player, float deltaTime);
        void setFollowSpeed(float speed) { mFollowSpeed = speed; }
        void setFollowDistance(float distance) { mFollowDistance = distance; }

    private:
        SDL_Texture* mSpriteSheetTexture;

        int mFacingDirection;
        bool mIsMoving;
        bool mIsFlying;

        float mFollowSpeed;
        float mFollowDistance;
        Vector mLastPlayerPosition;

        std::unordered_map<std::string, Animation> mAnimations;
        std::string mCurrentAnimation;

        const float DEFAULT_FOLLOW_SPEED = 120.0f;
        const float DEFAULT_FOLLOW_DISTANCE = 50.0f;
        const float STOP_DISTANCE = 30.0f;

        void loadAnimations();
        void updateAnimation();
        void updateMovement(const Vector& targetPosition, float deltaTime);
    };
}

#endif