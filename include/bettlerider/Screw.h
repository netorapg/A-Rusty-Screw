#ifndef _SCREW_H
#define _SCREW_H

#include <SDL2/SDL.h>
#include <memory>
#include "Object.h"
#include "Sprite.h"
#include "Globals.h"

namespace BRTC
{
    enum class ScrewType
    {
        FLATHEAD,
        PHILLIPS,
    };

    class Screw : public StaticObject
    {
        public:
        Screw(const Vector& postion,
                ScrewType type,
                SDL_Texture* textureSheet,
                SDL_Renderer* renderer);
        
        virtual ~Screw() = default;

        virtual void render(SDL_Renderer* renderer, Vector cameraPosition) override;

        void destroy() { mDestroyed = true; }

        bool isDestroyed() const { return mDestroyed; }

        SDL_Rect getBoundingBox() const;

        private:
        ScrewType mType;
        SpritePtr mSprite;
        bool mDestroyed;
    };
}

#endif // _SCREW_H