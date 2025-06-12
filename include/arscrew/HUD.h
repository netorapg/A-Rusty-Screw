#ifndef HUD_H
#define HUD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Player.h"
#include "Globals.h"

namespace ARSCREW
{
    class HUD
    {
    public:
        HUD(SDL_Renderer* renderer);
        ~HUD();
        
        void render(SDL_Renderer* renderer, const Player& player);
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
        
        // Posições dos elementos da HUD
        struct HUDPositions
        {
            SDL_Rect attackTypeBox;
            SDL_Rect instructionsBox;
            int attackTypeTextX, attackTypeTextY;
            int instructionsTextX, instructionsTextY;
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
        void renderBackground(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color, Uint8 alpha = 180);
        
        // Utilitário para criar textura de texto
        SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
    };
}

#endif // HUD_H