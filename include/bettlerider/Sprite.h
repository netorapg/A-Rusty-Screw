#ifndef BETTLERIDER_SPRITE_H
#define BETTLERIDER_SPRITE_H
#include <SDL2/SDL.h>
#include <memory>
#include "Animation.h"

namespace BRTC {
    class Sprite {
        public:
            Sprite(SDL_Renderer* renderer, const std::string& texturePath);

           // ~Sprite();

            void addAnimation(const std::string& name, Animation&& animation);
            void play(const std::string& animation, bool forceReset = false);
            void update(float deltaTime);
            void draw(SDL_Renderer* renderer, int x, int y, bool flip = false) const;

            SDL_Texture* getTexture() const;
            SDL_Point getSize() const;
        
        private:
            struct AnimationData {
                Animation animation;
                std::string currentAnimation;
            };

            SDL_Texture* texture = nullptr;
            SDL_Point size;
            std::unordered_map<std::string, Animation> animations;
            std::string currentAnimation;
    };
}
#endif