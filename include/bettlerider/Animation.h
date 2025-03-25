#ifndef BETTLERIDER_ANIMATION_H
#define BETTLERIDER_ANIMATION_H
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace BRTC
{
struct AnimationFrame {
    SDL_Rect rect;     
    float duration;    
    SDL_Point offset; 
};

class Animation {
public:
    Animation() = default;
    
    void addFrame(const AnimationFrame& frame);
    void setLoop(bool loop);
    bool isLooping() const;
    
    void update(float deltaTime);
    void reset();
    
    const SDL_Rect* getCurrentFrameRect() const;
    const SDL_Point* getCurrentOffset() const;
    bool isFinished() const;

private:
    std::vector<AnimationFrame> frames;
    size_t currentFrame = 0;
    float frameTimer = 0.0f;
    bool loop = true;
    bool finished = false;
};
}
#endif