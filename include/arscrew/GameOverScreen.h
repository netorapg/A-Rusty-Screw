#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Globals.h"

namespace ARSCREW
{
    enum class GameOverOption
    {
        RESTART,
        QUIT
    };

    class GameOverScreen
    {
    public:
        GameOverScreen(SDL_Renderer* renderer);
        ~GameOverScreen();

        void update(float deltaTime);
        void render(SDL_Renderer* renderer);
        void handleInput(const SDL_Event& event);

        GameOverOption getSelectedOption() const { return mSelectedOption; }
        bool isOptionConfirmed() const { return mOptionConfirmed; }
        void reset();

        // Efeitos visuais
        void startFadeIn();
        bool isFadeInComplete() const { return mFadeAlpha >= 255; }

    private:
        SDL_Renderer* mRenderer;
        TTF_Font* mTitleFont;
        TTF_Font* mOptionFont;

        // Texturas renderizadas
        SDL_Texture* mGameOverTexture;
        SDL_Texture* mRestartTexture;
        SDL_Texture* mQuitTexture;
        SDL_Texture* mInstructionTexture;

        // Estado da tela
        GameOverOption mSelectedOption;
        bool mOptionConfirmed;

        // Efeitos visuais
        float mFadeAlpha;
        float mFadeSpeed;
        float mBlinkTimer;
        bool mShowSelection;

        // Posições
        struct Positions
        {
            SDL_Rect gameOverRect;
            SDL_Rect restartRect;
            SDL_Rect quitRect;
            SDL_Rect instructionRect;
        } mPositions;

        // Cores
        SDL_Color mTextColor;
        SDL_Color mSelectedColor;
        SDL_Color mBackgroundColor;

        // Métodos auxiliares
        void initializeFonts();
        void createTextures();
        void destroyTextures();
        void calculatePositions();
        void renderBackground(SDL_Renderer* renderer);
        void renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
                         const SDL_Rect& rect, bool isSelected);
        SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
    };
}

#endif // GAMEOVERSCREEN_H
