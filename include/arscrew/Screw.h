#ifndef __BETTLE_RIDER_SCREW_H__
#define __BETTLE_RIDER_SCREW_H__

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>

namespace ARSCREW
{
    enum class ScrewType 
    {
        FLATHEAD,
        PHILLIPS
    };

    class Screw : public StaticObject 
    {
      public:
        Screw(const Vector& position,
              ScrewType type,
              SDL_Texture* textureSheet,
              SDL_Renderer* renderer);

        virtual ~Screw() = default;

        virtual void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        
        // Atualiza o timer de respawn
        void update(float deltaTime);

        void destroy() { mDestroyed = true; mRespawnTimer = mRespawnTime; }
        bool isDestroyed() const { return mDestroyed; }

        SDL_Rect getBoundingBox() const;
        ScrewType getType() const { return mType; }
        
        // Controle do sistema de respawn
        void setRespawnEnabled(bool enabled) { mRespawnEnabled = enabled; }
        bool isRespawnEnabled() const { return mRespawnEnabled; }
        void setRespawnTime(float time) { mRespawnTime = time; }
        float getRespawnTime() const { return mRespawnTime; }
        
      private:
        ScrewType   mType;
        SpritePtr   mSprite;
        bool        mDestroyed;
        
        // Sistema de respawn
        bool        mRespawnEnabled;
        float       mRespawnTime;
        float       mRespawnTimer;
        
        void respawn();
    };

} // namespace ARSCREW

#endif // __BETTLE_RIDER_SCREW_H__
