#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Object.h"
#include "Sprite.h"
#include "Animation.h"

namespace BRTC 
{
    class Platform : public StaticObject 
    {
        private:
            Animation mAnimation;
        public:
            Platform
            (
                const Vector position, 
                const Vector size, 
                SDL_Texture* texture, 
                int tileId
            );
            void render
            (
                SDL_Renderer* renderer, 
                Vector cameraPosition
            ) 
            override;
    };
}
#endif