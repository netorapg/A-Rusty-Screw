#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>
#include "../config.h"

class Tile {
public:
    Tile(int x, int y, int tileType);

    void render(SDL_Renderer* renderer, SDL_Texture* tileTexture, SDL_Rect* tileClips);

    int getType();

    SDL_Rect getBox();

private:
    SDL_Rect mBox;
    int mType;
};

#endif
