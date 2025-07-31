#include "../include/arscrew/HUD.h"
#include <iostream>
#include <vector>

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
        // Cores básicas
        mTextColor = {255, 255, 255, 255};        // Branco
        mBackgroundColor = {0, 0, 0, 180};        // Preto semi-transparente
        mCuttingColor = {0, 255, 0, 255};         // Verde para ataque cortante
        mPiercingColor = {0, 100, 255, 255};      // Azul para ataque perfurante
        mHealthBarColor = {0, 255, 0, 255};       // Verde para barra de vida do jogador
        mHealthBarBackgroundColor = {64, 64, 64, 255}; // Cinza escuro para fundo da barra
        mBossHealthBarColor = {255, 0, 0, 255};   // Vermelho para barra de vida do boss
        
        // Cores estilo retrô inspiradas em Super Metroid
        mRetroOrange = {255, 128, 0, 255};        // Laranja vibrante
        mRetroBlue = {0, 192, 255, 255};          // Azul ciano
        mRetroGreen = {0, 255, 128, 255};         // Verde neon
        mRetroRed = {255, 64, 64, 255};           // Vermelho brilhante
        mRetroCyan = {0, 255, 255, 255};          // Ciano brilhante
        mRetroYellow = {255, 255, 0, 255};        // Amarelo
        mRetroPurple = {192, 0, 255, 255};        // Roxo neon
        mRetroBackgroundDark = {16, 24, 32, 240}; // Azul escuro semi-transparente
        mRetroBorderLight = {128, 192, 255, 255}; // Azul claro para bordas
    }
    
    void HUD::initializePositions()
    {
        // Layout mais compacto estilo Super Metroid
        // Painel principal do jogador (canto superior esquerdo)
        mPositions.attackTypeBox = {8, 8, 180, 32};
        mPositions.attackTypeTextX = 16;
        mPositions.attackTypeTextY = 16;
        
        // Painel de instruções menor (canto superior direito)
        mPositions.instructionsBox = {SCREEN_WIDTH - 200, 8, 192, 72};
        mPositions.instructionsTextX = SCREEN_WIDTH - 192;
        mPositions.instructionsTextY = 12;
        
        // Painel de vida do jogador (abaixo do tipo de ataque)
        mPositions.playerHealthBox = {8, 48, 256, 48};
        mPositions.playerHealthTextX = 16;
        mPositions.playerHealthTextY = 56;
        
        // Painel do boss (parte inferior, mais largo)
        mPositions.bossHealthBox = {SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT - 72, 600, 64};
        mPositions.bossHealthTextX = SCREEN_WIDTH / 2 - 292;
        mPositions.bossHealthTextY = SCREEN_HEIGHT - 66;
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
        
        // Usar TTF_RenderText_Blended para evitar quebra de linha automática
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
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
    

    void HUD::setScore(float score)
    {
        mScore = score;
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

        // Score: visual retrô, sem casas decimais, destaque e sombra, com fundo
        char scoreBuffer[32];
        snprintf(scoreBuffer, sizeof(scoreBuffer), "SCORE: %d", static_cast<int>(std::round(mScore)));
        std::string scoreText = scoreBuffer;

        int scoreX = SCREEN_WIDTH - 1000;
        int scoreY = 15;

        // Medir tamanho do texto para ajustar o retângulo
        int textW = 0, textH = 0;
        if (mFont) {
            TTF_SizeText(mFont, scoreText.c_str(), &textW, &textH);
        } else {
            textW = 160; textH = 32; // fallback
        }
        SDL_Rect scoreRect = {scoreX - 16, scoreY - 8, textW + 32, textH + 16};
        renderRetroBackground(renderer, scoreRect, mRetroBackgroundDark, mRetroBorderLight);

        // Sombra mais sutil para melhor legibilidade
        SDL_Color shadowColor = {16, 32, 48, 180}; // sombra escura e translúcida
        renderRetroText(renderer, scoreText, scoreX + 1, scoreY + 1, shadowColor, true); // sombra sutil
        renderRetroText(renderer, scoreText, scoreX, scoreY, mRetroGreen, true); // texto principal
        
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
        // Renderizar fundo estilo retrô
        renderRetroBackground(renderer, mPositions.attackTypeBox, mRetroBackgroundDark, mRetroBorderLight);
        
        // Escolher cor e texto baseado no tipo de ataque
        SDL_Color attackColor;
        std::string attackText;
        
        if (currentAttack == AttackType::CUTTING)
        {
            attackColor = mRetroGreen;
            attackText = "CUTTING TOOL";
        }
        else if (currentAttack == AttackType::PIERCING)
        {
            attackColor = mRetroBlue;
            attackText = "PIERCING TOOL";
        }
        else
        {
            attackColor = mTextColor;
            attackText = "NO TOOL";
        }
        
        // Renderizar texto com estilo retrô
        renderRetroText(renderer, attackText, mPositions.attackTypeTextX, mPositions.attackTypeTextY, attackColor, false);
    }
    
    void HUD::renderInstructions(SDL_Renderer* renderer)
    {
        // Renderizar fundo estilo retrô
        renderRetroBackground(renderer, mPositions.instructionsBox, mRetroBackgroundDark, mRetroBorderLight);
        
        // Instruções com layout mais compacto
        std::vector<std::string> instructions = {
            "Q - SWITCH TOOL",
            "J - ATTACK",
            "E - DASH",
            "CTRL - DEBUG"
        };
        
        int lineHeight = 12;
        for (size_t i = 0; i < instructions.size(); i++)
        {
            renderRetroText(renderer, instructions[i], 
                           mPositions.instructionsTextX, 
                           mPositions.instructionsTextY + i * lineHeight, 
                           mRetroCyan, false);
        }
    }
    
    void HUD::renderPlayerHealth(SDL_Renderer* renderer, const Player& player)
    {
        // Renderizar fundo estilo retrô
        renderRetroBackground(renderer, mPositions.playerHealthBox, mRetroBackgroundDark, mRetroBorderLight);
        
        // Barra de energia segmentada (sem texto)
        SDL_Rect energyBarRect = {
            mPositions.playerHealthBox.x + 8,
            mPositions.playerHealthBox.y + 12,
            mPositions.playerHealthBox.w - 16,
            24
        };
        
        renderSegmentedHealthBar(renderer, energyBarRect, player.getCurrentHealth(), player.getMaxHealth(), mRetroGreen);
    }
    
    void HUD::renderBossHealth(SDL_Renderer* renderer, const Punktauro* boss)
    {
        if (!boss || boss->isDead()) return;
        
        // Renderizar fundo estilo retrô com cor mais intensa para o boss
        SDL_Color bossBgColor = {32, 16, 16, 240}; // Vermelho escuro
        renderRetroBackground(renderer, mPositions.bossHealthBox, bossBgColor, mRetroRed);
        
        // Nome do boss estilo retrô (apenas o nome)
        std::string bossName = "PUNKTAURO, O SADICO ROBO CANIBAL.";
        
        // Renderizar nome do boss mais à esquerda para caber o texto longo
        renderRetroText(renderer, bossName, 
                       mPositions.bossHealthBox.x + 12, 
                       mPositions.bossHealthTextY, mRetroRed, true);
        
        // Barra de vida do boss (mais larga e segmentada)
        SDL_Rect bossHealthBarRect = {
            mPositions.bossHealthBox.x + 8,
            mPositions.bossHealthBox.y + 32,
            mPositions.bossHealthBox.w - 16,
            24
        };
        
        renderSegmentedHealthBar(renderer, bossHealthBarRect, boss->getCurrentHealth(), boss->getMaxHealth(), mRetroPurple);
        
        // Efeito adicional: pulsação quando vida baixa
        float healthPercentage = static_cast<float>(boss->getCurrentHealth()) / static_cast<float>(boss->getMaxHealth());
        if (healthPercentage < 0.25f)
        {
            // Efeito de alerta piscando
            Uint32 time = SDL_GetTicks();
            if ((time / 250) % 2 == 0) // Pisca a cada 250ms
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 64);
                SDL_RenderFillRect(renderer, &mPositions.bossHealthBox);
            }
        }
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
    
    void HUD::renderRetroBackground(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color bgColor, SDL_Color borderColor)
    {
        // Fundo principal com gradiente simulado
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(renderer, &rect);
        
        // Bordas duplas estilo retrô
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        
        // Borda externa
        SDL_RenderDrawRect(renderer, &rect);
        
        // Borda interna (2 pixels para dentro)
        SDL_Rect innerRect = {rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4};
        SDL_SetRenderDrawColor(renderer, borderColor.r / 2, borderColor.g / 2, borderColor.b / 2, borderColor.a);
        SDL_RenderDrawRect(renderer, &innerRect);
        
        // Linha de destaque no topo
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        SDL_RenderDrawLine(renderer, rect.x + 1, rect.y + 1, rect.x + rect.w - 2, rect.y + 1);
        
        // Adicionar scan lines
        renderScanLines(renderer, rect);
    }

    void HUD::renderSegmentedHealthBar(SDL_Renderer* renderer, const SDL_Rect& barRect, int currentHealth, int maxHealth, SDL_Color barColor)
    {
        const int segmentCount = 20; // Número de segmentos
        const int segmentWidth = (barRect.w - 4) / segmentCount;
        const int segmentSpacing = 1;
        
        // Fundo da barra
        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        SDL_RenderFillRect(renderer, &barRect);
        
        // Calcular quantos segmentos devem estar preenchidos
        float healthPercentage = static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
        int filledSegments = static_cast<int>(healthPercentage * segmentCount);
        
        // Renderizar cada segmento
        for (int i = 0; i < segmentCount; i++)
        {
            SDL_Rect segmentRect = {
                barRect.x + 2 + i * (segmentWidth + segmentSpacing),
                barRect.y + 2,
                segmentWidth,
                barRect.h - 4
            };
            
            if (i < filledSegments)
            {
                // Segmento preenchido - cor varia baseada na vida
                SDL_Color segmentColor = barColor;
                if (healthPercentage < 0.25f) {
                    segmentColor = mRetroRed;
                } else if (healthPercentage < 0.5f) {
                    segmentColor = mRetroYellow;
                } else {
                    segmentColor = mRetroGreen;
                }
                
                SDL_SetRenderDrawColor(renderer, segmentColor.r, segmentColor.g, segmentColor.b, segmentColor.a);
                SDL_RenderFillRect(renderer, &segmentRect);
                
                // Destaque no topo do segmento
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
                SDL_RenderDrawLine(renderer, segmentRect.x, segmentRect.y, segmentRect.x + segmentRect.w - 1, segmentRect.y);
            }
            else
            {
                // Segmento vazio - apenas contorno
                SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
                SDL_RenderDrawRect(renderer, &segmentRect);
            }
        }
        
        // Borda da barra completa
        SDL_SetRenderDrawColor(renderer, mRetroBorderLight.r, mRetroBorderLight.g, mRetroBorderLight.b, mRetroBorderLight.a);
        SDL_RenderDrawRect(renderer, &barRect);
    }

    void HUD::renderScanLines(SDL_Renderer* renderer, const SDL_Rect& rect)
    {
        // Efeito de scan lines para dar sensação retrô
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
        for (int y = rect.y; y < rect.y + rect.h; y += 3)
        {
            SDL_RenderDrawLine(renderer, rect.x, y, rect.x + rect.w, y);
        }
    }

    void HUD::renderRetroText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, bool isLarge)
    {
        TTF_Font* font = isLarge ? mFont : mSmallFont;
        if (!font) return;
        
        // Criar textura temporária para o texto
        SDL_Texture* textTexture = createTextTexture(text, font, color);
        if (!textTexture) return;
        
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        
        SDL_Rect textRect = {x, y, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        
        SDL_DestroyTexture(textTexture);
    }
}