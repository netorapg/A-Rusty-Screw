#ifndef ARSCREW_PAUSE_MENU_H
#define ARSCREW_PAUSE_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Globals.h"

namespace ARSCREW
{
    enum class PauseMenuOption
    {
        RESUME,
        RESTART,
        MAIN_MENU,
        QUIT
    };

    struct PauseMenuPositions
    {
        SDL_Rect titleRect;
        SDL_Rect resumeRect;
        SDL_Rect restartRect;
        SDL_Rect mainMenuRect;
        SDL_Rect quitRect;
        SDL_Rect instructionRect;
    };

    class PauseMenu
    {
    public:
        explicit PauseMenu(SDL_Renderer* renderer);
        ~PauseMenu();

        void update(float deltaTime);
        void render(SDL_Renderer* renderer);
        void handleInput(const SDL_Event& event);
        
        void reset();
        void show();
        void hide();
        
        bool isOptionConfirmed() const { return mOptionConfirmed; }
        PauseMenuOption getSelectedOption() const { return mSelectedOption; }
        bool isVisible() const { return mVisible; }

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
        SDL_Texture* mResumeTexture;
        SDL_Texture* mRestartTexture;
        SDL_Texture* mMainMenuTexture;
        SDL_Texture* mQuitTexture;
        SDL_Texture* mInstructionTexture;
        
        PauseMenuPositions mPositions;
        PauseMenuOption mSelectedOption;
        bool mOptionConfirmed;
        bool mVisible;
        
        float mBlinkTimer;
        bool mShowSelection;
        
        SDL_Color mTextColor;
        SDL_Color mSelectedColor;
        SDL_Color mBackgroundColor;
    };
}

#endif // ARSCREW_PAUSE_MENU_H
