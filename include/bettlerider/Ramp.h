#ifndef RAMP_H
#define RAMP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Object.h"
#include "Sprite.h"
#include "Animation.h"

namespace BRTC 
{
    enum class RampType
    {
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        TOP_LEFT,
        TOP_RIGHT,
    };

    class Ramp : public StaticObject 
    {
        private:
            Animation mAnimation;
            RampType mType;
        public:
            Ramp
            (
                const Vector position, 
                const Vector size, 
                SDL_Texture* texture, 
                int tileId,
                RampType type
            );
            void render
            (   
                SDL_Renderer* renderer, 
                Vector cameraPosition
            ) 
            override;

            RampType getType() const {return mType;}
            bool isPointInside(const Vector& point) const;

              float getSurfaceY(float x) const {
        float relX = x - mPosition.x;
        
        switch(mType) {
            case RampType::BOTTOM_LEFT:
                return mPosition.y + mSize.y - (relX * mSize.y / mSize.x);
            case RampType::BOTTOM_RIGHT:
                return mPosition.y + (relX * mSize.y / mSize.x);
            case RampType::TOP_LEFT:
                return mPosition.y + (relX * mSize.y / mSize.x);
            case RampType::TOP_RIGHT:
                return mPosition.y + mSize.y - (relX * mSize.y / mSize.x);
            default:
                return mPosition.y;
        }
    }

    };
}
#endif