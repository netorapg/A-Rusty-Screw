#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include "../my-lib-master/include/my-lib/math-vector.h"
class Entity {
    public:
        Entity(float x, float y, int width, int height);
        virtual ~Entity() = default;
    
        // Métodos para obter e definir posição
        float getPosX() const;
        float getPosY() const;
        void setPosition(float x, float y);
    
        // Métodos para obter e definir velocidade
        float getHorizontalVelocity() const;
        float getVerticalVelocity() const;
        void setVelocity(float vx, float vy);
        void setHorizontalVelocity(float vx);
        void setVerticalVelocity(float vy);
    
        // Métodos para obter dimensões
        int getWidth() const;  // Adicione este método
        int getHeight() const; // Adicione este método
    
        // Métodos para gerenciar estados
        void setOnGround(bool onGround); // Adicione este método
        bool isPassingThroughPlatform() const;
        void setAboveCrate(bool aboveCrate);
    
        // Métodos virtuais
        virtual void update() = 0;
        virtual void render(SDL_Renderer* renderer, float cameraX, float cameraY) = 0;
        virtual void handleEvent(SDL_Event& e) = 0;

        bool isOnGround() const {
            return mOnGround;
        }

        void setFalling(bool falling) {
            mOnGround = !falling;
        }
    
    protected:
        float mPosX, mPosY; // Posição
        float mVelX, mVelY; // Velocidade
        int mWidth, mHeight; // Dimensões
        bool mPassingThroughPlatform; // Estado de passar por plataformas
        bool mAboveCrate; // Estado de estar acima de uma caixa
        bool mOnGround;   // Estado de estar no chão (adicione este membro)
    };
#endif // ENTITY_H