#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include <SDL2/SDL.h>

namespace BRTC
{
const float GRAVITY = 0.5f;

class Player : public DynamicObject {
    public:
        Player(Vector position, SDL_Renderer* renderer);
        ~Player();
    
        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        void handleEvent(SDL_Event& e) override;
        
        bool isFacingRight() const { return mFacingRight; }
    
        // Remova os setters redundantes (já existem em DynamicObject)
        // void setPassingThroughPlatform(bool state);  <-- Remover
        // void setFalling(bool state);  <-- Remover
    
        // Corrija para usar o tamanho do objeto (não do sprite)
        int getWidth() const { return static_cast<int>(mSize.x); }  // Corrigido
        int getHeight() const { return static_cast<int>(mSize.y); } // Corrigido
        void setPassingThroughPlatform(bool enable);
    
    private:
        SDL_Texture* mTexture;
        SDL_Rect mSpriteClip;
        int mCurrentFrame;
        int mFrameCount;
        float mAnimationTimer;
        float mAnimationSpeed;
        bool mFacingRight;
    
        static constexpr float GRAVITY = 0.5f;  // Mova para dentro da classe
    };
    
    } // namespace BRTC
    #endif