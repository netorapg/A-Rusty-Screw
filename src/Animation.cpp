#include "../include/bettlerider/Animation.h"

namespace BRTC
{
    void Animation::addFrame(const AnimationFrame& frame) {
        frames.push_back(frame);
    }
    
    void Animation::setLoop(bool loop) {
        this->loop = loop;
    }
    
    bool Animation::isLooping() const {
        return loop;
    }
    
    void Animation::update(float deltaTime) {
        if(frames.empty() || finished) return;
    
        frameTimer += deltaTime;
        
        while(frameTimer >= frames[currentFrame].duration) {
            frameTimer -= frames[currentFrame].duration;
            
            if(currentFrame + 1 < frames.size()) {
                currentFrame++;
            } else {
                if(loop) {
                    currentFrame = 0;
                } else {
                    finished = true;
                    break;
                }
            }
        }
    }
    
    void Animation::reset() {
        currentFrame = 0;
        frameTimer = 0.0f;
        finished = false;
    }
    
    const SDL_Rect* Animation::getCurrentFrameRect() const {
        return frames.empty() ? nullptr : &frames[currentFrame].rect;
    }
    
    const SDL_Point* Animation::getCurrentOffset() const {
        return frames.empty() ? nullptr : &frames[currentFrame].offset;
    }
    
    bool Animation::isFinished() const {
        return finished;
    }
}