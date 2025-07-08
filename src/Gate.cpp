#include "../include/arscrew/Gate.h"
#include <iostream>

namespace ARSCREW
{
    Gate::Gate(const Vector& position, 
               const Vector& size,
               SDL_Texture* texture,
               SDL_Renderer* renderer,
               Screw* targetScrew)
        : StaticObject(position, size)
        , mTargetScrew(targetScrew)
        , mState(GateState::CLOSED)
        , mOpeningSpeed(100.0f)  // 100 pixels por segundo
        , mCurrentOffset(0.0f)
        , mMaxOffset(size.y)     // A altura do portão é o offset máximo
        , mScrewWasDestroyed(false)
        , mClosedSprite(nullptr)
        , mOpenSprite(nullptr)
    {
        std::cout << "Gate created at position (" << position.x << ", " << position.y 
                  << ") with size (" << size.x << ", " << size.y << ")" << std::endl;
        
        if (mTargetScrew) {
            std::cout << "Gate linked to screw at position (" 
                      << mTargetScrew->getPosition().x << ", " 
                      << mTargetScrew->getPosition().y << ")" << std::endl;
        }
    }

    void Gate::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        switch (mState)
        {
            case GateState::CLOSED:
                // Renderizar portão completamente fechado
                renderGateWithStripes(renderer, cameraPosition, mPosition, mSize);
                break;
                
            case GateState::OPENING:
            case GateState::CLOSING:
                // Renderizar portão parcialmente aberto
                {
                    Vector renderPos = mPosition;
                    renderPos.y += mCurrentOffset;
                    Vector renderSize = mSize;
                    renderSize.y -= mCurrentOffset;
                    
                    if (renderSize.y > 0) {
                        renderGateWithStripes(renderer, cameraPosition, renderPos, renderSize);
                    }
                }
                break;
                
            case GateState::OPEN:
                // Portão completamente aberto - não renderizar nada
                break;
        }
    }

    void Gate::update(float deltaTime)
    {
        // Verificar status do parafuso alvo
        checkScrewStatus();
        
        // Atualizar animação baseada no estado
        switch (mState)
        {
            case GateState::OPENING:
                updateOpening(deltaTime);
                break;
                
            case GateState::CLOSING:
                updateClosing(deltaTime);
                break;
                
            case GateState::CLOSED:
            case GateState::OPEN:
                // Estados estáticos, nada para atualizar
                break;
        }
    }

    void Gate::checkScrewStatus()
    {
        if (!mTargetScrew) return;
        
        bool screwDestroyed = mTargetScrew->isDestroyed();
        
        // Se o parafuso foi destruído e o portão não está aberto/abrindo
        if (screwDestroyed && !mScrewWasDestroyed && (mState == GateState::CLOSED))
        {
            std::cout << "Target screw destroyed! Opening gate..." << std::endl;
            startOpening();
            mScrewWasDestroyed = true;
        }
        // Se o parafuso respawnou e o portão não está fechado/fechando
        else if (!screwDestroyed && mScrewWasDestroyed && (mState == GateState::OPEN))
        {
            std::cout << "Target screw respawned! Closing gate..." << std::endl;
            startClosing();
            mScrewWasDestroyed = false;
        }
    }

    void Gate::startOpening()
    {
        if (mState != GateState::CLOSED && mState != GateState::CLOSING) return;
        
        mState = GateState::OPENING;
        std::cout << "Gate starting to open..." << std::endl;
    }

    void Gate::startClosing()
    {
        if (mState != GateState::OPEN && mState != GateState::OPENING) return;
        
        mState = GateState::CLOSING;
        std::cout << "Gate starting to close..." << std::endl;
    }

    void Gate::updateOpening(float deltaTime)
    {
        mCurrentOffset += mOpeningSpeed * deltaTime;
        
        if (mCurrentOffset >= mMaxOffset)
        {
            mCurrentOffset = mMaxOffset;
            mState = GateState::OPEN;
            std::cout << "Gate fully opened!" << std::endl;
        }
    }

    void Gate::updateClosing(float deltaTime)
    {
        mCurrentOffset -= mOpeningSpeed * deltaTime;
        
        if (mCurrentOffset <= 0.0f)
        {
            mCurrentOffset = 0.0f;
            mState = GateState::CLOSED;
            std::cout << "Gate fully closed!" << std::endl;
        }
    }

    SDL_Rect Gate::getBoundingBox() const
    {
        if (mState == GateState::OPEN)
        {
            // Portão aberto não tem colisão
            return {0, 0, 0, 0};
        }
        else if (mState == GateState::OPENING || mState == GateState::CLOSING)
        {
            // Colisão apenas na parte visível do portão
            float visibleHeight = mSize.y - mCurrentOffset;
            return {
                static_cast<int>(mPosition.x),
                static_cast<int>(mPosition.y + mCurrentOffset),
                static_cast<int>(mSize.x),
                static_cast<int>(visibleHeight)
            };
        }
        else
        {
            // Portão fechado tem colisão completa
            return {
                static_cast<int>(mPosition.x),
                static_cast<int>(mPosition.y),
                static_cast<int>(mSize.x),
                static_cast<int>(mSize.y)
            };
        }
    }

    void Gate::renderGateWithStripes(SDL_Renderer* renderer, const Vector& cameraPosition, 
                                     const Vector& position, const Vector& size)
    {
        // Calcular posição na tela
        int screenX = static_cast<int>(position.x - cameraPosition.x);
        int screenY = static_cast<int>(position.y - cameraPosition.y);
        int width = static_cast<int>(size.x);
        int height = static_cast<int>(size.y);
        
        // Desenhar fundo cinza metálico
        SDL_SetRenderDrawColor(renderer, 140, 140, 140, 255); // Cinza metálico
        SDL_Rect backgroundRect = {screenX, screenY, width, height};
        SDL_RenderFillRect(renderer, &backgroundRect);
        
        // Desenhar sombra interna (efeito 3D)
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // Cinza muito escuro
        SDL_Rect shadowRect = {screenX + 2, screenY + 2, width - 2, height - 2};
        SDL_RenderDrawRect(renderer, &shadowRect);
        
        // Desenhar listras roxas diagonais
        SDL_SetRenderDrawColor(renderer, 148, 0, 211, 255); // Roxo vibrante (violet)
        
        int stripeWidth = 2; // Largura das listras
        int stripeSpacing = 6; // Espaçamento entre listras
        
        // Desenhar listras da esquerda superior para direita inferior
        for (int offset = -height; offset < width + height; offset += stripeSpacing)
        {
            // Calcular pontos da linha diagonal
            int startX = screenX + offset;
            int startY = screenY;
            int endX = screenX + offset + height;
            int endY = screenY + height;
            
            // Clipar as linhas para dentro do retângulo do portão
            if (startX < screenX) {
                int diff = screenX - startX;
                startX = screenX;
                startY += diff;
            }
            if (endX > screenX + width) {
                int diff = endX - (screenX + width);
                endX = screenX + width;
                endY -= diff;
            }
            if (startY > screenY + height || endY < screenY) continue;
            if (startY < screenY) {
                int diff = screenY - startY;
                startY = screenY;
                startX += diff;
            }
            if (endY > screenY + height) {
                int diff = endY - (screenY + height);
                endY = screenY + height;
                endX -= diff;
            }
            
            // Desenhar múltiplas linhas para criar espessura da listra
            for (int i = 0; i < stripeWidth; i++)
            {
                if (startX + i < screenX + width && startX + i >= screenX)
                {
                    SDL_RenderDrawLine(renderer, startX + i, startY, endX + i, endY);
                }
            }
        }
        
        // Adicionar listras roxas mais escuras para profundidade
        SDL_SetRenderDrawColor(renderer, 98, 0, 141, 255); // Roxo mais escuro
        for (int offset = -height + stripeSpacing/2; offset < width + height; offset += stripeSpacing)
        {
            int startX = screenX + offset;
            int startY = screenY;
            int endX = screenX + offset + height;
            int endY = screenY + height;
            
            // Clipar
            if (startX < screenX) {
                int diff = screenX - startX;
                startX = screenX;
                startY += diff;
            }
            if (endX > screenX + width) {
                int diff = endX - (screenX + width);
                endX = screenX + width;
                endY -= diff;
            }
            if (startY > screenY + height || endY < screenY) continue;
            if (startY < screenY) {
                int diff = screenY - startY;
                startY = screenY;
                startX += diff;
            }
            if (endY > screenY + height) {
                int diff = endY - (screenY + height);
                endY = screenY + height;
                endX -= diff;
            }
            
            // Linha mais fina para o efeito de profundidade
            if (startX < screenX + width && startX >= screenX)
            {
                SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
            }
        }
        
        // Borda externa preta
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Quase preto
        SDL_RenderDrawRect(renderer, &backgroundRect);
        
        // Highlight no topo e esquerda para efeito 3D
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Cinza claro
        SDL_RenderDrawLine(renderer, screenX + 1, screenY + 1, screenX + width - 2, screenY + 1); // Topo
        SDL_RenderDrawLine(renderer, screenX + 1, screenY + 1, screenX + 1, screenY + height - 2); // Esquerda
        
        // Parafusos decorativos nos cantos
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // Cinza escuro
        int screwSize = 3;
        // Canto superior esquerdo
        SDL_Rect screwTL = {screenX + 4, screenY + 4, screwSize, screwSize};
        SDL_RenderFillRect(renderer, &screwTL);
        // Canto superior direito
        SDL_Rect screwTR = {screenX + width - 4 - screwSize, screenY + 4, screwSize, screwSize};
        SDL_RenderFillRect(renderer, &screwTR);
        // Canto inferior esquerdo
        SDL_Rect screwBL = {screenX + 4, screenY + height - 4 - screwSize, screwSize, screwSize};
        SDL_RenderFillRect(renderer, &screwBL);
        // Canto inferior direito
        SDL_Rect screwBR = {screenX + width - 4 - screwSize, screenY + height - 4 - screwSize, screwSize, screwSize};
        SDL_RenderFillRect(renderer, &screwBR);
    }

} // namespace ARSCREW
