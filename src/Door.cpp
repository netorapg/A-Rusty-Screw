#include "../include/bettlerider/Door.h"

namespace BRTC
{
    Door::Door
    (
        const Vector position, 
        const Vector size, 
        const std::string& levelToLoad, 
        SDL_Renderer* renderer, 
        const std::string& texturePath, 
        Vector spawnPosition
    ) 
    : StaticObject(position, size),
      mLevelToLoad(levelToLoad),
      mSpawnPosition(spawnPosition)
    {
    SDL_Texture* texture = IMG_LoadTexture(renderer, texturePath.c_str());
    if (!texture) 
    {
        SDL_Log("Falha ao carregar textura: %s", SDL_GetError());
        return;
    }
    SpritePtr doorSprite = std::make_shared<Sprite>
    (
        texture, 
        SDL_Rect
        {
            64, 
            192, 
            static_cast<int>(size.x), 
            static_cast<int>(size.y)
        }
    );
    mAnimation.addFrame({doorSprite, 0.0f, {0, 0}});
    mAnimation.setLoop(false);
    }
    std::string Door::getLevelToLoad() const { return mLevelToLoad; }
    Vector Door::getSpawnPosition() const { return mSpawnPosition; }
    void Door::render(SDL_Renderer* renderer, Vector cameraPosition) 
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
