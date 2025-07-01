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
        mHealthBarColor = {0, 255, 0, 255};       // Verde para barra de vida do jogador
        mHealthBarBackgroundColor = {64, 64, 64, 255}; // Cinza escuro para fundo da barra
        mBossHealthBarColor = {255, 0, 0, 255};   // Vermelho para barra de vida do boss
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
        
        // Caixa de vida do jogador (parte superior central)
        mPositions.playerHealthBox = {SCREEN_WIDTH / 2 - 100, 10, 200, 50};
        mPositions.playerHealthTextX = SCREEN_WIDTH / 2 - 90;
        mPositions.playerHealthTextY = 15;
        
        // Caixa de vida do boss (parte inferior central)
        mPositions.bossHealthBox = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 80, 300, 60};
        mPositions.bossHealthTextX = SCREEN_WIDTH / 2 - 140;
        mPositions.bossHealthTextY = SCREEN_HEIGHT - 75;
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
    
    void HUD::render(SDL_Renderer* renderer, const Player& player, const Punktauro* boss)
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
        
        // Renderizar vida do jogador
        renderPlayerHealth(renderer, player);
        
        // Renderizar vida do boss se existir e não estiver morto
        if (boss && !boss->isDead())
        {
            renderBossHealth(renderer, boss);
        }
        
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
    
    void HUD::renderPlayerHealth(SDL_Renderer* renderer, const Player& player)
    {
        // Renderizar fundo
        renderBackground(renderer, mPositions.playerHealthBox, mBackgroundColor);
        
        // Texto de vida do jogador
        std::string healthText = "Player HP: " + std::to_string(player.getCurrentHealth()) + "/" + std::to_string(player.getMaxHealth());
        SDL_Texture* healthTexture = createTextTexture(healthText, mFont, mTextColor);
        
        if (healthTexture)
        {
            int textWidth, textHeight;
            SDL_QueryTexture(healthTexture, nullptr, nullptr, &textWidth, &textHeight);
            
            SDL_Rect textRect = {
                mPositions.playerHealthTextX,
                mPositions.playerHealthTextY,
                textWidth,
                textHeight
            };
            
            SDL_RenderCopy(renderer, healthTexture, nullptr, &textRect);
            SDL_DestroyTexture(healthTexture);
        }
        
        // Barra de vida
        SDL_Rect healthBarRect = {
            mPositions.playerHealthBox.x + 10,
            mPositions.playerHealthBox.y + 30,
            mPositions.playerHealthBox.w - 20,
            15
        };
        
        renderHealthBar(renderer, healthBarRect, player.getCurrentHealth(), player.getMaxHealth(), mHealthBarColor);
    }
    
    void HUD::renderBossHealth(SDL_Renderer* renderer, const Punktauro* boss)
    {
        if (!boss || boss->isDead()) return;
        
        // Renderizar fundo
        renderBackground(renderer, mPositions.bossHealthBox, mBackgroundColor);
        
        // Texto de vida do boss com nome e fase
        std::string bossText = "PUNKTAURO (Phase " + std::to_string(static_cast<int>(boss->getCurrentPhase())) + ")";
        std::string healthText = std::to_string(boss->getCurrentHealth()) + "/" + std::to_string(boss->getMaxHealth()) + " HP";
        
        SDL_Texture* bossNameTexture = createTextTexture(bossText, mFont, mTextColor);
        SDL_Texture* healthTexture = createTextTexture(healthText, mSmallFont, mTextColor);
        
        // Renderizar nome do boss
        if (bossNameTexture)
        {
            int textWidth, textHeight;
            SDL_QueryTexture(bossNameTexture, nullptr, nullptr, &textWidth, &textHeight);
            
            SDL_Rect textRect = {
                mPositions.bossHealthTextX,
                mPositions.bossHealthTextY,
                textWidth,
                textHeight
            };
            
            SDL_RenderCopy(renderer, bossNameTexture, nullptr, &textRect);
            SDL_DestroyTexture(bossNameTexture);
        }
        
        // Renderizar vida numérica
        if (healthTexture)
        {
            int textWidth, textHeight;
            SDL_QueryTexture(healthTexture, nullptr, nullptr, &textWidth, &textHeight);
            
            SDL_Rect textRect = {
                mPositions.bossHealthBox.x + mPositions.bossHealthBox.w - textWidth - 10,
                mPositions.bossHealthTextY,
                textWidth,
                textHeight
            };
            
            SDL_RenderCopy(renderer, healthTexture, nullptr, &textRect);
            SDL_DestroyTexture(healthTexture);
        }
        
        // Barra de vida do boss (maior)
        SDL_Rect healthBarRect = {
            mPositions.bossHealthBox.x + 10,
            mPositions.bossHealthBox.y + 35,
            mPositions.bossHealthBox.w - 20,
            20
        };
        
        renderHealthBar(renderer, healthBarRect, boss->getCurrentHealth(), boss->getMaxHealth(), mBossHealthBarColor);
    }
    
    void HUD::renderHealthBar(SDL_Renderer* renderer, const SDL_Rect& barRect, int currentHealth, int maxHealth, SDL_Color barColor)
    {
        // Renderizar fundo da barra de vida
        SDL_SetRenderDrawColor(renderer, mHealthBarBackgroundColor.r, mHealthBarBackgroundColor.g, mHealthBarBackgroundColor.b, mHealthBarBackgroundColor.a);
        SDL_RenderFillRect(renderer, &barRect);
        
        // Calcular largura da barra de vida baseada na porcentagem
        float healthPercentage = static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
        if (healthPercentage < 0.0f) healthPercentage = 0.0f;
        if (healthPercentage > 1.0f) healthPercentage = 1.0f;
        
        int healthBarWidth = static_cast<int>(barRect.w * healthPercentage);
        
        // Mudar cor da barra baseado na vida restante
        SDL_Color finalBarColor = barColor;
        if (healthPercentage < 0.25f) {
            // Vermelho quando vida baixa
            finalBarColor = {255, 0, 0, 255};
        } else if (healthPercentage < 0.5f) {
            // Amarelo quando vida média
            finalBarColor = {255, 255, 0, 255};
        }
        
        // Renderizar barra de vida preenchida
        if (healthBarWidth > 0)
        {
            SDL_Rect filledRect = {barRect.x, barRect.y, healthBarWidth, barRect.h};
            SDL_SetRenderDrawColor(renderer, finalBarColor.r, finalBarColor.g, finalBarColor.b, finalBarColor.a);
            SDL_RenderFillRect(renderer, &filledRect);
        }
        
        // Renderizar borda da barra
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &barRect);
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