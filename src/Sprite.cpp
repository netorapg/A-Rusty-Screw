#include "../include/bettlerider/Sprite.h"
#include <SDL2/SDL_image.h>
#include <stdexcept>

namespace BRTC {
    Sprite::Sprite(SDL_Renderer* renderer, const std::string& texturePath) {
        SDL_Surface* surface = IMG_Load(texturePath.c_str());
        if(!surface) {
            throw std::runtime_error("Failed to load texture: " + std::string(IMG_GetError()));
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        size = {surface->w, surface->h};
        SDL_FreeSurface(surface);
    }

    void Sprite::addAnimation(const std::string& name, Animation&& animation) {
        animations.emplace(name, std::move(animation));
    }

    void Sprite::play(const std::string& animation, bool forceReset) {
        if (animation == currentAnimation && !forceReset) return;

        auto it = animations.find(animation);
        if(it != animations.end()) {
            currentAnimation = animation;
            it->second.reset();
        }
    }

    void Sprite::update(float deltaTime) {
        auto it = animations.find(currentAnimation);
        if(it != animations.end()) {
            it->second.update(deltaTime);
        }
    }

    void Sprite::draw(SDL_Renderer* renderer, int x, int y, bool flip) const {
        auto it = animations.find(currentAnimation);
        if(it == animations.end()) return;

        const SDL_Rect* frame = it->second.getCurrentFrameRect();
        const SDL_Point* offset = it->second.getCurrentOffset();

        if(!frame || !texture) return;

        SDL_Rect dest = {
            x + offset->x,
            y + offset->y,
            frame->w,
            frame->h
        };

        SDL_RendererFlip flipFlag = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, texture, frame, &dest, 0.0, nullptr, flipFlag);
    }

    SDL_Texture* Sprite::getTexture() const {
        return texture;
    }

    SDL_Point Sprite::getSize() const {
        return size;
    }
} // namespace BRTC