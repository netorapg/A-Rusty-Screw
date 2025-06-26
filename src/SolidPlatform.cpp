#include "../include/arscrew/SolidPlatform.h"

namespace ARSCREW 
{
    SolidPlatform::SolidPlatform
    (
        const Vector position, 
        const Vector size, 
        SDL_Texture* texture, 
        int tileId
    ) : StaticObject(position, size)
    {
        const int tilesetColumns = 15;
        const int tileWidth = 32;
        const int tileHeight = 32;
        int relativeId = tileId - 1;
        int tilesetX = (relativeId % tilesetColumns) * tileWidth;
        int tilesetY = (relativeId / tilesetColumns) * tileHeight;
        SpritePtr platformSprite =  std::make_shared<Sprite>
        (
            texture, 
            SDL_Rect
            {
                tilesetX, 
                tilesetY, 
                static_cast<int>(size.x), 
                static_cast<int>(size.y)
            }
        );
        mAnimation.addFrame({platformSprite, 0.0f, {0, 0}});
        mAnimation.setLoop(false);
    }
    void SolidPlatform::render
    (
        SDL_Renderer* renderer, 
        Vector cameraPosition
    ) 
    {
    const Vector screenPosition = mPosition - cameraPosition;
    SpritePtr currentSprite = mAnimation.getCurrentSprite();
    if (currentSprite) 
        {
         //   int finalScreenX = static_cast<int>(screenPosition.x);
         //   int finalScreenY = static_cast<int>(screenPosition.y);
            currentSprite->draw(renderer, screenPosition.x, screenPosition.y);
        }
    }
}