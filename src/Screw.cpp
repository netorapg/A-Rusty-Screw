#include "../include/arscrew/Screw.h"
#include <stdexcept>

namespace ARSCREW 
{

    Screw::Screw(const Vector& position,
                 ScrewType type,
                 SDL_Texture* textureSheet,
                 SDL_Renderer* renderer)
        : StaticObject(position, Vector(16.0f, 16.0f)), // supondo que cada sprite ocupe 16�16 px
          mType(type),
          mDestroyed(false)
    {
        // Determina qual ret�ngulo do spritesheet vamos usar:
        SDL_Rect srcRect;
        switch (mType)
        {
            case ScrewType::FLATHEAD:
                // Suponha que, no seu spritesheet "screws_spritesheet.png", 
                // o parafuso tipo "fenda" esteja em (x=0, y=0, w=16, h=16)
                srcRect = SDL_Rect{ 0, 0, 16, 16 };
                break;

            case ScrewType::PHILLIPS:
                // Suponha que o parafuso "estrela" esteja em (x=16, y=0, w=16, h=16)
                srcRect = SDL_Rect{ 16, 0, 16, 16 };
                break;

            default:
                throw std::runtime_error("Screw::Screw: tipo de parafuso inv�lido");
        }

        // Cria o Sprite que ir� desenhar apenas esse sub-ret�ngulo do textureSheet
        mSprite = std::make_shared<Sprite>(textureSheet, srcRect);

        // Ajusta o mSize (caso queira variar, por hora mantemos 16�16)
        mSize = Vector(static_cast<float>(srcRect.w),
                       static_cast<float>(srcRect.h));
    }

    SDL_Rect Screw::getBoundingBox() const
    {
        return SDL_Rect {
            static_cast<int>(mPosition.x),
            static_cast<int>(mPosition.y),
            static_cast<int>(mSize.x),
            static_cast<int>(mSize.y)
        };
    }

    void Screw::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        if (mDestroyed) 
            return;

        // Se n�o est� vis�vel na janela, nem desenha
        if (!isVisible(cameraPosition, Vector(SCREEN_WIDTH, SCREEN_HEIGHT))) 
            return;

        // Converte coordenadas de mundo para coordenadas de tela
        Vector screenPos = mPosition - cameraPosition;
        int dstX = static_cast<int>(screenPos.x);
        int dstY = static_cast<int>(screenPos.y);

        // Renderiza o sprite (flip = false, pois n�o queremos inverter)
        mSprite->draw(renderer, dstX, dstY, false);
    }

} // namespace BRTC
