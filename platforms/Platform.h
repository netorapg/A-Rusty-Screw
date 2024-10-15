#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include "../Object.h"

class Platform : public Object
{
public:
    Platform(float x, float y, float width, float height);

    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
    bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight); // Adicionando o método isVisible
};

#endif // PLATFORM_H
