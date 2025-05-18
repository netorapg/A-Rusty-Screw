#include "../include/bettlerider/Ramp.h"

namespace BRTC 
{
    Ramp::Ramp(
        const Vector position, 
        const Vector size, 
        SDL_Texture* texture, 
        int tileId,
        RampType type
    ) : StaticObject(position, size), mType(type)
    {
        const int tilesetColumns = 15;
        const int tileWidth = 32;
        const int tileHeight = 32;
        int relativeId = tileId - 1;
        int tilesetX = (relativeId % tilesetColumns) * tileWidth;
        int tilesetY = (relativeId / tilesetColumns) * tileHeight;
        
        SpritePtr rampSprite = std::make_shared<Sprite>(
            texture, 
            SDL_Rect{
                tilesetX, 
                tilesetY, 
                static_cast<int>(size.x), 
                static_cast<int>(size.y)
            }
        );
        
        mAnimation.addFrame({rampSprite, 0.0f, {0, 0}});
        mAnimation.setLoop(false);
    }

    void Ramp::render(SDL_Renderer* renderer, Vector cameraPosition) 
    {
        const Vector screenPosition = mPosition - cameraPosition;
        SpritePtr currentSprite = mAnimation.getCurrentSprite();
        
        if (currentSprite) 
        {
            // Renderiza normalmente (a textura já deve ter o formato triangular)
            currentSprite->draw(renderer, screenPosition.x, screenPosition.y);
        }
    }

    bool Ramp::isPointInside(const Vector& point) const 
    {
        // Coordenadas relativas ao canto inferior esquerdo do objeto
        float relX = point.x - mPosition.x;
        float relY = point.y - mPosition.y;
        
        // Equação da hipotenusa (considerando triângulo retângulo de pernas em X e Y)
        switch(mType) {
            case RampType::BOTTOM_LEFT:
                // Hipotenusa do canto inferior esquerdo ao superior direito
                return (relX >= 0) && (relY >= 0) && 
                       ((relY / mSize.y) <= (1 - relX / mSize.x));
                
            case RampType::BOTTOM_RIGHT:
                // Hipotenusa do canto inferior direito ao superior esquerdo
                return (relX <= mSize.x) && (relY >= 0) && 
                       ((relY / mSize.y) <= (relX / mSize.x));
                
            case RampType::TOP_LEFT:
                // Hipotenusa do canto superior esquerdo ao inferior direito
                return (relX >= 0) && (relY <= mSize.y) && 
                       ((relY / mSize.y) >= (relX / mSize.x));
                
            case RampType::TOP_RIGHT:
                // Hipotenusa do canto superior direito ao inferior esquerdo
                return (relX <= mSize.x) && (relY <= mSize.y) && 
                       ((relY / mSize.y) >= (1 - relX / mSize.x));
                
            default:
                return false;
        }
    }
}