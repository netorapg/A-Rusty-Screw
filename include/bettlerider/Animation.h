#ifndef BETTLERIDER_ANIMATION_H
#define BETTLERIDER_ANIMATION_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <iostream>
#include "Sprite.h"
#include "Globals.h"

namespace BRTC
{
    struct AnimationFrame 
    {
    SpritePtr sprite;     
    float duration;    
    SDL_Point offset; 
    };
    class Animation 
    {
    public:
        Animation() = default;
        void addFrame(const AnimationFrame& frame);
        void setLoop(bool loop);
        void update(float deltaTime);
        void reset();
        bool isLooping() const;
        bool isFinished() const;
        const SDL_Point* getCurrentOffset() const;
        SpritePtr getCurrentSprite() const;
    private:
        std::vector<AnimationFrame> frames;
        size_t currentFrame = 0;
        float frameTimer = 0.0f;
        bool loop = true;
        bool finished = false;
    };
}
#endif