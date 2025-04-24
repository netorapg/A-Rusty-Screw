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
    };
}
#endif