#include "../include/bettlerider/Screw.h"
#include <stdexcept>

namespace BRTC
{

    // Construtor: 
    //  position    = posição (esquerda, topo) em coordenadas de mundo
    //  type        = determina qual sub-rect do spritesheet usar
    //  textureSheet= a SDL_Texture* já carregada contendo todos os parafusos
    //  renderer    = necessário para criar o Sprite a partir do textureSheet
    Screw::Screw(const Vector& position,
                 ScrewType type,
                 SDL_Texture* textureSheet,
                 SDL_Renderer* renderer)
        : StaticObject(position, Vector(16, 16)), // supondo que cada frame seja 16×16
          mType(type),
          mDestroyed(false)
    {
        // Escolhe o srcRect dentro do spritesheet de acordo com mType
        SDL_Rect src;
        switch (mType) 
        {
            case ScrewType::FLATHEAD:
                // Exemplo: no seu spritesheet, o parafuso “fenda” está em (0, 0, 16, 16)
                src = SDL_Rect{ 0, 0, 16, 16 };
                break;
            case ScrewType::PHILLIPS:
                // Exemplo: “estrela” está em (16, 0, 16, 16)
                src = SDL_Rect{ 16, 0, 16, 16 };
                break;
            default:
                throw std::runtime_error("Screw: tipo de parafuso inválido");
        }

        // Cria um Sprite compartilhado que usa 'textureSheet' + 'src'
        mSprite = std::make_shared<Sprite>(textureSheet, src);

        // Ajusta mSize para o tamanho real do sprite (caso queira variar)
        mSize = Vector(static_cast<float>(src.w),
                       static_cast<float>(src.h));
    }

    SDL_Rect Screw::getBoundingBox() const
    {
        // Retângulo de colisão em coordenadas de mundo
        return SDL_Rect{
            static_cast<int>(mPosition.x),
            static_cast<int>(mPosition.y),
            static_cast<int>(mSize.x),
            static_cast<int>(mSize.y)
        };
    }

    void Screw::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        if (mDestroyed) return;

        // Se não estiver visível na câmera, não desenha
        if (!isVisible(cameraPosition, Vector(SCREEN_WIDTH, SCREEN_HEIGHT)))
            return;

        // Converte posição de mundo para posição na tela
        Vector screenPos = mPosition - cameraPosition;
        int dstX = static_cast<int>(screenPos.x);
        int dstY = static_cast<int>(screenPos.y);

        // Desenha o sprite na tela (flip = false)
        mSprite->draw(renderer, dstX, dstY, false);
    }

} // namespace BRTC
