#include "../include/bettlerider/Decoration.h"

namespace BRTC 
{
    Decoration::Decoration
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
        SpritePtr decorationSprite = std::make_shared<Sprite>
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
        mAnimation.addFrame({decorationSprite, 0.0f, {0, 0}});
        mAnimation.setLoop(false);
    }
    void Decoration::render
        (
            SDL_Renderer* renderer, 
            Vector cameraPosition
        ) 
    {
        const Vector screenPosition = mPosition - cameraPosition;
        SpritePtr currentSprite = mAnimation.getCurrentSprite();
        if (currentSprite) 
        {
        currentSprite->draw
        (
            renderer, 
            screenPosition.x, 
            screenPosition.y
        );
        }
    }
}