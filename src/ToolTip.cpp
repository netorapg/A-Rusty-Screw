#include "../include/arscrew/ToolTip.h"
#include <iostream>
#include <cmath>

namespace ARSCREW
{
    ToolTip::ToolTip(const Vector& position, 
                     ToolTipType type,
                     SDL_Texture* texture,
                     SDL_Renderer* renderer)
        : StaticObject(position, Vector(24, 24)) // Tamanho padrão 24x24
        , mType(type)
        , mCollected(false)
        , mFloating(true)
        , mFloatTimer(0.0f)
        , mFloatAmplitude(8.0f)   // 8 pixels de amplitude
        , mFloatSpeed(3.0f)       // 3 radianos por segundo
        , mOriginalPosition(position)
    {
        // Criar sprite baseado no tipo, similar aos parafusos
        if (texture) {
            SDL_Rect srcRect;
            switch (mType)
            {
                case ToolTipType::FLATHEAD:
                    // Ponta Flathead - assumindo posição (0, 0) na textura weapon.png
                    srcRect = SDL_Rect{ 0, 0, 24, 24 };
                    break;

                case ToolTipType::PHILLIPS:
                    // Ponta Phillips - assumindo posição (24, 0) na textura weapon.png
                    srcRect = SDL_Rect{ 0, 0, 24, 24 };
                    break;

                default:
                    // Fallback para o primeiro sprite
                    srcRect = SDL_Rect{ 0, 0, 24, 24 };
                    break;
            }
            
            mSprite = std::make_shared<Sprite>(texture, srcRect);
        }
        
        std::cout << "ToolTip created at position (" << position.x << ", " << position.y 
                  << ") of type " << (type == ToolTipType::FLATHEAD ? "FLATHEAD" : "PHILLIPS") 
                  << " with " << (texture ? "sprite" : "custom render") << std::endl;
    }

    void ToolTip::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        if (mCollected) return; // Não renderizar se já foi coletada

        Vector renderPosition = mPosition;
        
        // Aplicar efeito de flutuação se ativado
        if (mFloating) {
            renderPosition.y = mOriginalPosition.y + std::sin(mFloatTimer) * mFloatAmplitude;
        }

        if (mSprite) {
            // Usar sprite se disponível
            mSprite->draw(renderer, 
                         static_cast<int>(renderPosition.x - cameraPosition.x), 
                         static_cast<int>(renderPosition.y - cameraPosition.y));
        } else {
            // Renderizar visual customizado se não há sprite
            renderToolTipCustom(renderer, cameraPosition, renderPosition, mSize);
        }
    }

    void ToolTip::update(float deltaTime)
    {
        if (mCollected) return;
        
        // Atualizar timer de flutuação
        if (mFloating) {
            mFloatTimer += mFloatSpeed * deltaTime;
            
            // Manter o timer em um range razoável para evitar overflow
            if (mFloatTimer > 2 * M_PI) {
                mFloatTimer -= 2 * M_PI;
            }
            
            // Atualizar posição para flutuação
            mPosition.y = mOriginalPosition.y + std::sin(mFloatTimer) * mFloatAmplitude;
        }
    }

    SDL_Rect ToolTip::getBoundingBox() const
    {
        if (mCollected) {
            return {0, 0, 0, 0}; // Sem colisão se coletada
        }
        
        return {
            static_cast<int>(mPosition.x),
            static_cast<int>(mPosition.y),
            static_cast<int>(mSize.x),
            static_cast<int>(mSize.y)
        };
    }

    void ToolTip::renderToolTipCustom(SDL_Renderer* renderer, const Vector& cameraPosition, 
                                     const Vector& position, const Vector& size)
    {
        // Calcular posição na tela
        int screenX = static_cast<int>(position.x - cameraPosition.x);
        int screenY = static_cast<int>(position.y - cameraPosition.y);
        int width = static_cast<int>(size.x);
        int height = static_cast<int>(size.y);
        
        // Cores baseadas no tipo
        if (mType == ToolTipType::FLATHEAD) {
            // Ponta de fenda - amarelo dourado
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Dourado
        } else {
            // Ponta Phillips - azul metálico
            SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // Azul aço
        }
        
        // Desenhar fundo da ponta
        SDL_Rect toolRect = {screenX + 2, screenY + 2, width - 4, height - 4};
        SDL_RenderFillRect(renderer, &toolRect);
        
        // Desenhar borda escura
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Cinza escuro
        SDL_RenderDrawRect(renderer, &toolRect);
        
        // Desenhar padrão específico do tipo
        if (mType == ToolTipType::FLATHEAD) {
            // Linha horizontal para fenda
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Marrom escuro
            SDL_RenderDrawLine(renderer, 
                              screenX + 4, screenY + height/2, 
                              screenX + width - 4, screenY + height/2);
            SDL_RenderDrawLine(renderer, 
                              screenX + 4, screenY + height/2 + 1, 
                              screenX + width - 4, screenY + height/2 + 1);
        } else {
            // Cruz para Phillips
            SDL_SetRenderDrawColor(renderer, 25, 25, 112, 255); // Azul escuro
            // Linha horizontal
            SDL_RenderDrawLine(renderer, 
                              screenX + 6, screenY + height/2, 
                              screenX + width - 6, screenY + height/2);
            // Linha vertical
            SDL_RenderDrawLine(renderer, 
                              screenX + width/2, screenY + 6, 
                              screenX + width/2, screenY + height - 6);
        }
        
        // Efeito de brilho (apenas se estiver flutuando)
        if (mFloating) {
            // Brilho sutil no canto superior esquerdo
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128); // Branco translúcido
            SDL_Rect glowRect = {screenX + 3, screenY + 3, 4, 4};
            SDL_RenderFillRect(renderer, &glowRect);
        }
    }

} // namespace ARSCREW
