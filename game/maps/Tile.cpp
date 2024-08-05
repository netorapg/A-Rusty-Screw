#include "Tile.h"

Tile::Tile(int x, int y, int tileType) {
    mBox.x = x;
    mBox.y = y;
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;
    mType = tileType;
}

void Tile::render(SDL_Renderer* renderer, SDL_Texture* tileTexture, SDL_Rect* tileClips) {
    SDL_Rect renderQuad = { mBox.x, mBox.y, TILE_WIDTH, TILE_HEIGHT };
    SDL_RenderCopy(renderer, tileTexture, &tileClips[mType], &renderQuad);
}

int Tile::getType() {
    return mType;
}

SDL_Rect Tile::getBox() {
    return mBox;
}
