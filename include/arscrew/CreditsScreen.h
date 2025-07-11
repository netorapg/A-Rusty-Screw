#ifndef ARSCREW_CREDITS_SCREEN_H
#define ARSCREW_CREDITS_SCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "Globals.h"

namespace ARSCREW
{
    enum class CreditsOption
    {
        MAIN_MENU,
        QUIT
    };

    struct CreditLine
    {
        std::string text;
        int fontSize;
        SDL_Color color;
        float yOffset;
        SDL_Texture* texture;
    };

    class CreditsScreen
    {
    public:
        explicit CreditsScreen(SDL_Renderer* renderer);
        ~CreditsScreen();

        void update(float deltaTime);
        void render(SDL_Renderer* renderer);
        void handleInput(const SDL_Event& event);
        
        void reset();
        void startCredits();
        void skipToEnd();
        
        bool isOptionConfirmed() const { return mOptionConfirmed; }
        CreditsOption getSelectedOption() const { return mSelectedOption; }
        bool isFinished() const { return mShowOptions; }

    private:
        void initializeFonts();
        void createCreditLines();
        void createOptionTextures();
        void destroyTextures();
        SDL_Texture* createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color);
        
        void renderBackground(SDL_Renderer* renderer);
        void renderCredits(SDL_Renderer* renderer);
        void renderOptions(SDL_Renderer* renderer);
        void renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
                         const SDL_Rect& rect, bool isSelected);

        SDL_Renderer* mRenderer;
        TTF_Font* mTitleFont;
        TTF_Font* mNormalFont;
        TTF_Font* mSmallFont;
        
        std::vector<CreditLine> mCreditLines;
        SDL_Texture* mMainMenuTexture;
        SDL_Texture* mQuitTexture;
        SDL_Texture* mInstructionTexture;
        
        CreditsOption mSelectedOption;
        bool mOptionConfirmed;
        bool mShowOptions;
        
        float mScrollOffset;
        float mScrollSpeed;
        float mFadeAlpha;
        float mFadeSpeed;
        float mBlinkTimer;
        bool mShowSelection;
        
        SDL_Color mTextColor;
        SDL_Color mSelectedColor;
        SDL_Color mBackgroundColor;
    };
}

#endif // ARSCREW_CREDITS_SCREEN_H
