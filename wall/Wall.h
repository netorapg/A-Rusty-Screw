// Wall.h
#ifndef WALL_H
#define WALL_H

#include <SDL2/SDL.h>
#include "../Object.h"

class Wall : public Object
{
public:
    Wall(float x, float y, float width, float height);

    void render(SDL_Renderer *renderer, float cameraX, float cameraY) override; // Adicione os parâmetros da câmera
};

#endif // WALL_H
