#ifndef DOOR_H
#define DOOR_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "Object.h"
#include "Sprite.h"
#include "Animation.h"

namespace BRTC
{
    class Door : public StaticObject 
    {
        private:
            Animation mAnimation;
            std::string mLevelToLoad;
            Vector mSpawnPosition;
        public: 
            Door
            (
                const Vector position, 
                const Vector size, 
                const std::string& levelToLoad, 
                SDL_Renderer* renderer, 
                const std::string& texturePath, 
                Vector spawnPosition
            );
            std::string getLevelToLoad() const;
            Vector getSpawnPosition() const;
            void render(SDL_Renderer* renderer, Vector cameraPosition) override;
            bool isPlayerColliding(const DynamicObject& player) const;
            bool hasValidSpawn() const 
            {
                return (mSpawnPosition.x >= 0 && mSpawnPosition.y >= 0);
            }
    };
}
#endif
