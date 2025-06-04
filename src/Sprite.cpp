#include <SDL2/SDL_image.h>
#include <stdexcept>
#include "../include/arscrew/Sprite.h"

namespace ARSCREW 
{   
    Sprite::Sprite(SDL_Renderer* renderer, const std::string& texturePath) 
    {
        SDL_Surface* surface = IMG_Load(texturePath.c_str());
        if (!surface) 
        {
            throw std::runtime_error("Failed to load texture: " + std::string(IMG_GetError()));
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) 
        {
            SDL_FreeSurface(surface);
            throw std::runtime_error("Failed to create texture: " + std::string(IMG_GetError()));
        }
        srcRect = { 0, 0, surface->w, surface->h };
        ownsTexture = true;
        SDL_FreeSurface(surface);
    }

    Sprite::Sprite(SDL_Texture* texture, const SDL_Rect& srcRect)
        : texture(texture), srcRect(srcRect), ownsTexture(false) {}
    
    Sprite::~Sprite() 
    {
        if (ownsTexture && texture) {
            SDL_DestroyTexture(texture);
        }
    }

    void Sprite::draw
    (
        SDL_Renderer* renderer, 
        int x, 
        int y, 
        bool flip
    ) const 
            {
                SDL_Rect dest = { x, y, srcRect.w, srcRect.h };
                SDL_RendererFlip flipFlag = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                SDL_RenderCopyEx(renderer, texture, &srcRect, &dest, 0.0, nullptr, flipFlag);
            }

    SDL_Point Sprite::getSize() const { return { srcRect.w, srcRect.h }; }
    SDL_Texture* Sprite::getTexture() const { return texture; }
    const SDL_Rect& Sprite::getSrcRect() const { return srcRect; }
} 