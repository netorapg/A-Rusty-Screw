#ifndef DOOR_H
#define DOOR_H

#include "../../scenario/Scenario.h"
#include "../../entity/Entity.h"
#include <string>
#include <SDL2/SDL.h>

namespace BRTC
{

class Door : public Scenario {
    public: 
        Door(float x, float y, float width, float height, const std::string& levelToLoad); 
        
        bool isPlayerColliding(const Entity& player) const;
    
        std::string getLevelToLoad() const;
    
        void render(SDL_Renderer* renderer, float cameraX, float cameraY) override;
    
    private:
        std::string mLevelToLoad;
    };
}

#endif // DOOR_H
