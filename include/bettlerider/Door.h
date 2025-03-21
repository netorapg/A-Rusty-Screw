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
    public: 
        Door(const Vector position, const Vector size, const std::string& levelToLoad, SDL_Renderer* renderer, const std::string& texturePath);
        
        bool isPlayerColliding(const DynamicObject& player) const;
    
        std::string getLevelToLoad() const;
    
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
    
    private:
        std::string mLevelToLoad;
    };
}

#endif // DOOR_H
