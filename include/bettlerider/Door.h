#ifndef DOOR_H
#define DOOR_H

#include "Object.h"
#include <string>
#include <SDL2/SDL.h>

namespace BRTC
{

class Door : public StaticObject {
    public: 
        Door(float x, float y, float width, float height, const std::string& levelToLoad); 
        
        bool isPlayerColliding(const DynamicObject& player) const;
    
        std::string getLevelToLoad() const;
    
        void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
    
    private:
        std::string mLevelToLoad;
    };
}

#endif // DOOR_H
