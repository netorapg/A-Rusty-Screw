#include "../include/bettlerider/Wall.h"

namespace BRTC
{

Wall::Wall(const Vector position, const Vector size, SDL_Texture* texture, int tileId)
    : StaticObject(position, size)
{
      // Considerando firstgid=1 e tileset com 15 colunas
      const int tilesetColumns = 15;
      const int tileWidth = 32;
      const int tileHeight = 32;
      
      // Calcula o índice relativo ao tileset
      int relativeId = tileId - 1; // Subtrai o firstgid (1)
      
      // Calcula posição no tileset
      int tilesetX = (relativeId % tilesetColumns) * tileWidth;
      int tilesetY = (relativeId / tilesetColumns) * tileHeight;
      
    SpritePtr wallSprite =  std::make_shared<Sprite>(
        texture, 
        SDL_Rect{tilesetX, tilesetY, static_cast<int>(size.x), static_cast<int>(size.y)}
    );

    mAnimation.addFrame({wallSprite, 0.0f, {0, 0}});
    mAnimation.setLoop(false);
}

void Wall::render(SDL_Renderer* renderer, Vector cameraPosition) {
    const Vector screenPosition = mPosition - cameraPosition;
    SpritePtr currentSprite = mAnimation.getCurrentSprite();
    if (currentSprite) {
        currentSprite->draw(renderer, screenPosition.x, screenPosition.y);
    }
}

}