#ifndef ARSCREW_START_MENU_H
#define ARSCREW_START_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Globals.h"

namespace ARSCREW
{
    enum class StartMenuOption
    {
        START_GAME,
        QUIT
    };

    struct StartMenuPositions
    {
        SDL_Rect titleRect;
        SDL_Rect startRect;
        SDL_Rect quitRect;
        SDL_Rect instructionRect;
    };

    class StartMenu
    {
    public:
        explicit StartMenu(SDL_Renderer* renderer);
        ~StartMenu();

        void update(float deltaTime);
        void render(SDL_Renderer* renderer);
        void handleInput(const SDL_Event& event);
        
        void reset();
        void startFadeIn();
        
        bool isOptionConfirmed() const { return mOptionConfirmed; }
        StartMenuOption getSelectedOption() const { return mSelectedOption; }

    private:
        void initializeFonts();
        void calculatePositions();
        void createTextures();
        void destroyTextures();
        SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
        
        void renderBackground(SDL_Renderer* renderer);
        void renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
                         const SDL_Rect& rect, bool isSelected);

        SDL_Renderer* mRenderer;
        TTF_Font* mTitleFont;
        TTF_Font* mOptionFont;
        
        SDL_Texture* mTitleTexture;
        SDL_Texture* mStartTexture;
        SDL_Texture* mQuitTexture;
        SDL_Texture* mInstructionTexture;
        
        StartMenuPositions mPositions;
        StartMenuOption mSelectedOption;
        bool mOptionConfirmed;
        
        float mFadeAlpha;
        float mFadeSpeed;
        float mBlinkTimer;
        bool mShowSelection;
        
        SDL_Color mTextColor;
        SDL_Color mSelectedColor;
        SDL_Color mBackgroundColor;
    };
}

#endif // ARSCREW_START_MENU_H
