#ifndef WALL_H
#define WALL_H

#include <SDL2/SDL.h>
#include "../Scenario.h"

class Wall : public Scenario
{
public:
    Wall(float x, float y, float width, float height);
    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
    bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) override;
};

#endif // WALL_H
