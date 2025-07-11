#ifndef HUD_H
#define HUD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Player.h"
#include "Punktauro.h"
#include "Globals.h"

namespace ARSCREW
{
    class HUD
    {
    public:
        HUD(SDL_Renderer* renderer);
        ~HUD();
        
        void render(SDL_Renderer* renderer, const Player& player, const Punktauro* boss = nullptr);
        void update(float deltaTime);
        
        // Configurações visuais
        void setVisible(bool visible) { mVisible = visible; }
        bool isVisible() const { return mVisible; }
        
    private:
        SDL_Renderer* mRenderer;
        TTF_Font* mFont;
        TTF_Font* mSmallFont;
        
        bool mVisible;
        
        // Cores para diferentes elementos
        SDL_Color mTextColor;
        SDL_Color mBackgroundColor;
        SDL_Color mCuttingColor;
        SDL_Color mPiercingColor;
        SDL_Color mHealthBarColor;
        SDL_Color mHealthBarBackgroundColor;
        SDL_Color mBossHealthBarColor;
        
        // Cores estilo retrô
        SDL_Color mRetroOrange;
        SDL_Color mRetroBlue;
        SDL_Color mRetroGreen;
        SDL_Color mRetroRed;
        SDL_Color mRetroCyan;
        SDL_Color mRetroYellow;
        SDL_Color mRetroPurple;
        SDL_Color mRetroBackgroundDark;
        SDL_Color mRetroBorderLight;
        
        // Posições dos elementos da HUD
        struct HUDPositions
        {
            SDL_Rect attackTypeBox;
            SDL_Rect instructionsBox;
            SDL_Rect playerHealthBox;
            SDL_Rect bossHealthBox;
            int attackTypeTextX, attackTypeTextY;
            int instructionsTextX, instructionsTextY;
            int playerHealthTextX, playerHealthTextY;
            int bossHealthTextX, bossHealthTextY;
        } mPositions;
        
        // Texturas renderizadas para otimização
        SDL_Texture* mCuttingTexture;
        SDL_Texture* mPiercingTexture;
        SDL_Texture* mInstructionsTexture;
        
        // Métodos auxiliares
        void initializeFonts();
        void initializeColors();
        void initializePositions();
        void createTextTextures();
        void destroyTextTextures();
        
        // Métodos de renderização
        void renderAttackType(SDL_Renderer* renderer, AttackType currentAttack);
        void renderInstructions(SDL_Renderer* renderer);
        void renderPlayerHealth(SDL_Renderer* renderer, const Player& player);
        void renderBossHealth(SDL_Renderer* renderer, const Punktauro* boss);
        void renderBackground(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color, Uint8 alpha = 180);
        void renderHealthBar(SDL_Renderer* renderer, const SDL_Rect& barRect, int currentHealth, int maxHealth, SDL_Color barColor);
        
        // Métodos de renderização estilo retrô
        void renderRetroBackground(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color bgColor, SDL_Color borderColor);
        void renderSegmentedHealthBar(SDL_Renderer* renderer, const SDL_Rect& barRect, int currentHealth, int maxHealth, SDL_Color barColor);
        void renderScanLines(SDL_Renderer* renderer, const SDL_Rect& rect);
        void renderRetroText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, bool isLarge = false);
        
        // Utilitário para criar textura de texto
        SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
    };
}

#endif // HUD_H