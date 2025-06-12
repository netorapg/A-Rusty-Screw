#include "../include/arscrew/HUD.h"
#include <iostream>

namespace ARSCREW
{
    HUD::HUD(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mFont(nullptr)
        , mSmallFont(nullptr)
        , mVisible(true)
        , mCuttingTexture(nullptr)
        , mPiercingTexture(nullptr)
        , mInstructionsTexture(nullptr)
    {
        initializeFonts();
        initializeColors();
        initializePositions();
        createTextTextures();
    }
    
    HUD::~HUD()
    {
        destroyTextTextures();
        
        if (mFont) TTF_CloseFont(mFont);
        if (mSmallFont) TTF_CloseFont(mSmallFont);
    }
    
    void HUD::initializeFonts()
    {
        // Tenta carregar fontes diferentes, com fallback
        const char* fontPaths[] = {
            "../assets/fonts/Arial.ttf",
            "../assets/fonts/Megadeth.ttf",
            "../assets/fonts/METALORD.TTF",
        };
        
        for (const char* fontPath : fontPaths)
        {
            mFont = TTF_OpenFont(fontPath, 16);
            if (mFont) break;
        }
        
        for (const char* fontPath : fontPaths)
        {
            mSmallFont = TTF_OpenFont(fontPath, 12);
            if (mSmallFont) break;
        }
        
        if (!mFont || !mSmallFont)
        {
            std::cerr << "Failed to load fonts for HUD: " << TTF_GetError() << std::endl;
        }
    }
    
    void HUD::initializeColors()
    {
        mTextColor = {255, 255, 255, 255};        // Branco
        mBackgroundColor = {0, 0, 0, 180};        // Preto semi-transparente
        mCuttingColor = {0, 255, 0, 255};         // Verde para ataque cortante
        mPiercingColor = {0, 100, 255, 255};      // Azul para ataque perfurante
    }
    
    void HUD::initializePositions()
    {
        // Caixa do tipo de ataque (canto superior esquerdo)
        mPositions.attackTypeBox = {10, 10, 200, 40};
        mPositions.attackTypeTextX = 20;
        mPositions.attackTypeTextY = 20;
        
        // Caixa de instruções (canto superior direito)
        mPositions.instructionsBox = {SCREEN_WIDTH - 220, 10, 210, 80};
        mPositions.instructionsTextX = SCREEN_WIDTH - 210;
        mPositions.instructionsTextY = 15;
    }
    
    void HUD::createTextTextures()
    {
        if (!mFont || !mSmallFont) return;
        
        // Criar texturas de texto para os tipos de ataque
        mCuttingTexture = createTextTexture("CUTTING TOOL", mFont, mCuttingColor);
        mPiercingTexture = createTextTexture("PIERCING TOOL", mFont, mPiercingColor);
        
        // Criar textura de instruções
        std::string instructions = "Q - Switch Tool\nJ - Attack\nE - Dash\nCtrl - Debug";
        mInstructionsTexture = createTextTexture(instructions, mSmallFont, mTextColor);
    }
    
    void HUD::destroyTextTextures()
    {
        if (mCuttingTexture) SDL_DestroyTexture(mCuttingTexture);
        if (mPiercingTexture) SDL_DestroyTexture(mPiercingTexture);
        if (mInstructionsTexture) SDL_DestroyTexture(mInstructionsTexture);
        
        mCuttingTexture = nullptr;
        mPiercingTexture = nullptr;
        mInstructionsTexture = nullptr;
    }
    
    SDL_Texture* HUD::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color)
    {
        if (!font) return nullptr;
        
        // Para texto com múltiplas linhas, vamos usar TTF_RenderText_Blended
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, 300);
        if (!textSurface)
        {
            std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
            return nullptr;
        }
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        SDL_FreeSurface(textSurface);
        
        if (!texture)
        {
            std::cerr << "Unable to create texture from rendered text: " << SDL_GetError() << std::endl;
        }
        
        return texture;
    }
    
    void HUD::update(float deltaTime)
    {
        // Aqui podemos adicionar animações ou atualizações futuras da HUD
    }
    
    void HUD::render(SDL_Renderer* renderer, const Player& player)
    {
        if (!mVisible) return;
        
        // Salvar o blend mode atual
        SDL_BlendMode currentBlendMode;
        SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        // Renderizar tipo de ataque atual
        renderAttackType(renderer, player.getCurrentAttackType());
        
        // Renderizar instruções
        renderInstructions(renderer);
        
        // Restaurar blend mode
        SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
    }
    
    void HUD::renderAttackType(SDL_Renderer* renderer, AttackType currentAttack)
    {
        // Renderizar fundo
        renderBackground(renderer, mPositions.attackTypeBox, mBackgroundColor);
        
        // Escolher textura baseada no tipo de ataque
        SDL_Texture* textTexture = nullptr;
        if (currentAttack == AttackType::CUTTING)
        {
            textTexture = mCuttingTexture;
        }
        else if (currentAttack == AttackType::PIERCING)
        {
            textTexture = mPiercingTexture;
        }
        
        // Renderizar texto
        if (textTexture)
        {
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            
            SDL_Rect textRect = {
                mPositions.attackTypeTextX,
                mPositions.attackTypeTextY,
                textWidth,
                textHeight
            };
            
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        }
    }
    
    void HUD::renderInstructions(SDL_Renderer* renderer)
    {
        // Renderizar fundo
        renderBackground(renderer, mPositions.instructionsBox, mBackgroundColor);
        
        // Renderizar texto de instruções
        if (mInstructionsTexture)
        {
            int textWidth, textHeight;
            SDL_QueryTexture(mInstructionsTexture, nullptr, nullptr, &textWidth, &textHeight);
            
            SDL_Rect textRect = {
                mPositions.instructionsTextX,
                mPositions.instructionsTextY,
                textWidth,
                textHeight
            };
            
            SDL_RenderCopy(renderer, mInstructionsTexture, nullptr, &textRect);
        }
    }
    
    void HUD::renderBackground(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color, Uint8 alpha)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
        SDL_RenderFillRect(renderer, &rect);
        
        // Adicionar borda
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
        SDL_RenderDrawRect(renderer, &rect);
    }
}