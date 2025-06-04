#include "../include/arscrew/Animation.h"

namespace ARSCREW
{
    bool Animation::isLooping() const { return loop;}
    bool Animation::isFinished() const { return finished;}
    void Animation::addFrame(const AnimationFrame& frame) { frames.push_back(frame); }
    void Animation::setLoop(bool loop) { this->loop = loop; }
    void Animation::update(float deltaTime) 
    {
        if (frames.empty() || finished) return;
        frameTimer += deltaTime;
        while (frameTimer >= frames[currentFrame].duration) 
        {
            frameTimer -= frames[currentFrame].duration;

            if (currentFrame + 1 < frames.size()) {currentFrame++; } 
            else 
            {
                if (loop) { currentFrame = 0; } 
                else 
                { finished = true;
                break;
                }
            }
        }
    }
    void Animation::reset() 
    {
       currentFrame = 0;
       frameTimer = 0.0f;
       finished = false;
    }
    const SDL_Point* Animation::getCurrentOffset() const 
    {
        static SDL_Point defaultOffset = {0, 0};
        try 
        { return &frames.at(currentFrame).offset; } 
        catch (const std::out_of_range& e) 
        {
            std::cerr << "Erro: Índice fora do intervalo em getCurrentOffset()!\n";
            return &defaultOffset;
        }
    }

   SpritePtr Animation::getCurrentSprite() const 
   {
       try 
       { return frames.at(currentFrame).sprite; } 
       catch (const std::out_of_range& e) 
       {
           SDL_Log("Erro: Índice fora do intervalo em getCurrentSprite()!");
           return nullptr;
       }
   }
}
