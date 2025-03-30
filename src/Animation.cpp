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

   void Animation::update(float deltaTime) { // deltaTime passado como parâmetro
    if (frames.empty() || finished) return;

    frameTimer += deltaTime;

    while (frameTimer >= frames[currentFrame].duration) {
        frameTimer -= frames[currentFrame].duration; // Subtrai a duração

        if (currentFrame + 1 < frames.size()) {
            currentFrame++;
        } else {
            if (loop) {
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

   const SDL_Point* Animation::getCurrentOffset() const {
    static SDL_Point defaultOffset = {0, 0};
    try {
        return &frames.at(currentFrame).offset;  // Retorna o endereço do offset
    } catch (const std::out_of_range& e) {
        std::cerr << "Erro: Índice fora do intervalo em getCurrentOffset()!\n";
        return &defaultOffset;
    }
}


   SpritePtr Animation::getCurrentSprite() const {
       try {
           SDL_Log("Frame atual: %d", currentFrame);
           return frames.at(currentFrame).sprite;  // Alterado para `at()`
       } catch (const std::out_of_range& e) {
           SDL_Log("Erro: Índice fora do intervalo em getCurrentSprite()!");
           return nullptr;
       }
   }

   bool Animation::isFinished() const {
       return finished;
   }
}
