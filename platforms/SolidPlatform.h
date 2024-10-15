#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include <SDL2/SDL.h>
#include "../Object.h"

class SolidPlatform : public Object
{
public:
    SolidPlatform(float x, float y, float width, float height);

    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; 
    bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight); // Adicionando o método isVisible
};

#endif // SOLIDPLATFORM_H
