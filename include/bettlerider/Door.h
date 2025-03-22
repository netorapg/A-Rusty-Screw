#ifndef DOOR_H
#define DOOR_H

#include "Object.h"
#include <string>
#include <SDL2/SDL.h>
#include "Sprite.h"

namespace BRTC
{

class Door : public StaticObject {
    private:
        Sprite mSprite;
        std::string mLevelToLoad;
        Vector mSpawnPosition;
    public: 
        Door(const Vector position, const Vector size, const std::string& levelToLoad, SDL_Renderer* renderer, const std::string& texturePath, Vector spawnPosition);
        
        bool isPlayerColliding(const DynamicObject& player) const;
    
        std::string getLevelToLoad() const;

        Vector getSpawnPosition() const;
    
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;

        bool hasValidSpawn() const {
            return (mSpawnPosition.x >= 0 && mSpawnPosition.y >= 0);
        }
    
    };
}

#endif // DOOR_H
