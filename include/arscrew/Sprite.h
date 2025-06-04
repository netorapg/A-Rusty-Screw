#ifndef BETTLERIDER_SPRITE_H
#define BETTLERIDER_SPRITE_H

#include <SDL2/SDL.h>
#include <memory>
#include <array>

namespace ARSCREW
{
    class Sprite 
    {
        public:
            Sprite
            (
                SDL_Renderer* renderer, 
                const std::string& texturePath
            );
            Sprite
            (
                SDL_Texture* texture, 
                const SDL_Rect& srcRect
            );
            ~Sprite();
            void draw
            (
                SDL_Renderer* renderer, 
                int x, 
                int y, 
                bool flip = false
            ) 
            const;
            SDL_Point getSize() const;
            SDL_Texture* getTexture() const;
            const SDL_Rect& getSrcRect() const;
            private:
            SDL_Texture* texture = nullptr;
            SDL_Rect srcRect;
            bool ownsTexture = false;
    };
    using SpritePtr = std::shared_ptr<Sprite>;
}
#endif