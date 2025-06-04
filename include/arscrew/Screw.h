#ifndef __BETTLE_RIDER_SCREW_H__
#define __BETTLE_RIDER_SCREW_H__

#include "Object.h"      // contém StaticObject, Vector, etc.
#include "Sprite.h"      // contém Sprite, SpritePtr
#include <SDL2/SDL.h>

namespace ARSCREW
{
    // Enum para os tipos de parafuso (cada tipo usará uma sub-região diferente do spritesheet)
    enum class ScrewType 
    {
        FLATHEAD,   // "fenda"
        PHILLIPS    // "estrela"
        // Se quiser adicionar outros tipos, basta incluir aqui e ajustar em Screw.cpp
    };

    class Screw : public StaticObject 
    {
      public:
        // Construtor recebe:
        //  - position: posição (x,y) em coordenadas de mundo
        //  - type: tipo de parafuso (FLATHEAD ou PHILLIPS)
        //  - textureSheet: o SDL_Texture* já carregado contendo ambas as imagens de screws
        //  - renderer: necessário para criar o Sprite a partir do textureSheet
        Screw(const Vector& position,
              ScrewType type,
              SDL_Texture* textureSheet,
              SDL_Renderer* renderer);

        virtual ~Screw() = default;

        // Renderiza o sprite na tela, ajustando pela posição da câmera
        virtual void render(SDL_Renderer* renderer, Vector cameraPosition) override;

        // Marca o parafuso como destruído (não será renderizado nem colidido)
        void destroy() { mDestroyed = true; }

        bool isDestroyed() const { return mDestroyed; }

        // Retorna o retângulo AABB em coordenadas de mundo para checar colisão
        SDL_Rect getBoundingBox() const;

      private:
        ScrewType   mType;
        SpritePtr   mSprite;     // Aponta para a sub-região certa do spritesheet
        bool        mDestroyed;  // Se true, o screwing não será mais desenhado
    };

} // namespace BRTC

#endif // __BETTLE_RIDER_SCREW_H__
