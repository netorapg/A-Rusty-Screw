#include "../include/arscrew/GameOverScreen.h"
#include <iostream>

namespace ARSCREW
{
    GameOverScreen::GameOverScreen(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mTitleFont(nullptr)
        , mOptionFont(nullptr)
        , mGameOverTexture(nullptr)
        , mRestartTexture(nullptr)
        , mQuitTexture(nullptr)
        , mInstructionTexture(nullptr)
        , mSelectedOption(GameOverOption::RESTART)
        , mOptionConfirmed(false)
        , mFadeAlpha(0.0f)
        , mFadeSpeed(300.0f)
        , mBlinkTimer(0.0f)
        , mShowSelection(true)
    {
        initializeFonts();
        calculatePositions();
        createTextures();

        // Cores
        mTextColor = {255, 255, 255, 255};        // Branco puro para melhor visibilidade
        mSelectedColor = {255, 255, 0, 255};      // Amarelo brilhante para seleção
        mBackgroundColor = {0, 0, 0, 200};
    }

    GameOverScreen::~GameOverScreen()
    {
        destroyTextures();
        if (mTitleFont) TTF_CloseFont(mTitleFont);
        if (mOptionFont) TTF_CloseFont(mOptionFont);
    }

    void GameOverScreen::initializeFonts()
    {
        const char* fontPaths[] = {
            "../assets/fonts/Megadeth.ttf",
            "../assets/fonts/METALORD.TTF",
            "../assets/fonts/Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux fallback
            "/System/Library/Fonts/Arial.ttf",                  // macOS fallback
            "C:\\Windows\\Fonts\\arial.ttf"                     // Windows fallback
        };

        for (const char* fontPath : fontPaths)
        {
            mTitleFont = TTF_OpenFont(fontPath, 48);
            if (mTitleFont) {
                std::cout << "GameOverScreen: Loaded title font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "GameOverScreen: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        for (const char* fontPath : fontPaths)
        {
            mOptionFont = TTF_OpenFont(fontPath, 24);
            if (mOptionFont) {
                std::cout << "GameOverScreen: Loaded option font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "GameOverScreen: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        if (!mTitleFont || !mOptionFont)
        {
            std::cerr << "CRITICAL: Failed to load any fonts for Game Over screen!" << std::endl;
        }
    }

    void GameOverScreen::calculatePositions()
    {
        // Game Over título (centro superior)
        mPositions.gameOverRect = {
            SCREEN_WIDTH / 2 - 150,
            SCREEN_HEIGHT / 2 - 100,
            300, 60
        };

        // Opção Restart
        mPositions.restartRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2,
            200, 40
        };

        // Opção Quit
        mPositions.quitRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 + 60,
            200, 40
        };

        // Instruções
        mPositions.instructionRect = {
            SCREEN_WIDTH / 2 - 150,
            SCREEN_HEIGHT / 2 + 140,
            300, 30
        };
    }

    void GameOverScreen::createTextures()
    {
        if (!mTitleFont || !mOptionFont) {
            std::cerr << "GameOverScreen: Cannot create textures - fonts not loaded!" << std::endl;
            return;
        }

        std::cout << "GameOverScreen: Creating textures..." << std::endl;
        
        mGameOverTexture = createTextTexture("GAME OVER", mTitleFont, {255, 0, 0, 255});
        mRestartTexture = createTextTexture("RESTART", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mQuitTexture = createTextTexture("QUIT", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mInstructionTexture = createTextTexture("Use W/S to select, ENTER to confirm", 
                                               mOptionFont, {220, 220, 220, 255}); // Cinza claro
        
        std::cout << "GameOverScreen: Textures created - GameOver: " << (mGameOverTexture ? "OK" : "FAIL") 
                  << ", Restart: " << (mRestartTexture ? "OK" : "FAIL")
                  << ", Quit: " << (mQuitTexture ? "OK" : "FAIL")
                  << ", Instructions: " << (mInstructionTexture ? "OK" : "FAIL") << std::endl;
    }

    void GameOverScreen::destroyTextures()
    {
        if (mGameOverTexture) SDL_DestroyTexture(mGameOverTexture);
        if (mRestartTexture) SDL_DestroyTexture(mRestartTexture);
        if (mQuitTexture) SDL_DestroyTexture(mQuitTexture);
        if (mInstructionTexture) SDL_DestroyTexture(mInstructionTexture);

        mGameOverTexture = nullptr;
        mRestartTexture = nullptr;
        mQuitTexture = nullptr;
        mInstructionTexture = nullptr;
    }

    SDL_Texture* GameOverScreen::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color)
    {
        if (!font) {
            std::cerr << "GameOverScreen: Cannot create texture - font is null!" << std::endl;
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "GameOverScreen: Failed to create text surface for '" << text << "': " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        if (!texture) {
            std::cerr << "GameOverScreen: Failed to create texture for '" << text << "': " << SDL_GetError() << std::endl;
        } else {
            std::cout << "GameOverScreen: Successfully created texture for '" << text << "'" << std::endl;
        }

        return texture;
    }

    void GameOverScreen::update(float deltaTime)
    {
        // Fade in effect
        if (mFadeAlpha < 255.0f)
        {
            mFadeAlpha += mFadeSpeed * deltaTime;
            if (mFadeAlpha > 255.0f) mFadeAlpha = 255.0f;
        }

        // Blink effect for selected option
        mBlinkTimer += deltaTime;
        if (mBlinkTimer >= 0.5f)
        {
            mShowSelection = !mShowSelection;
            mBlinkTimer = 0.0f;
        }
    }

    void GameOverScreen::render(SDL_Renderer* renderer)
    {
        // Renderizar background escuro
        renderBackground(renderer);

        // Debug log
        static bool firstRender = true;
        static int renderCount = 0;
        renderCount++;
        
        if (firstRender || renderCount % 60 == 0) { // Log a cada segundo (60 FPS)
            std::cout << "GameOverScreen: Render #" << renderCount << " - FadeAlpha: " << mFadeAlpha << std::endl;
            firstRender = false;
        }

        // Só renderizar se o fade in começou
        if (mFadeAlpha > 0.0f)
        {
            Uint8 alpha = static_cast<Uint8>(mFadeAlpha);

            // Renderizar título "GAME OVER"
            if (mGameOverTexture)
            {
                SDL_SetTextureAlphaMod(mGameOverTexture, alpha);
                SDL_RenderCopy(renderer, mGameOverTexture, nullptr, &mPositions.gameOverRect);
            }

            // Renderizar opções
            renderOption(renderer, mRestartTexture, mPositions.restartRect, 
                        mSelectedOption == GameOverOption::RESTART);
            
            renderOption(renderer, mQuitTexture, mPositions.quitRect, 
                        mSelectedOption == GameOverOption::QUIT);

            // Renderizar instruções
            if (mInstructionTexture)
            {
                SDL_SetTextureAlphaMod(mInstructionTexture, alpha);
                SDL_RenderCopy(renderer, mInstructionTexture, nullptr, &mPositions.instructionRect);
            }
        }
    }

    void GameOverScreen::renderBackground(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, mBackgroundColor.r, mBackgroundColor.g, 
                              mBackgroundColor.b, static_cast<Uint8>(mFadeAlpha * 0.8f));
        
        SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    void GameOverScreen::renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
                                     const SDL_Rect& rect, bool isSelected)
    {
        if (!texture) return;

        Uint8 alpha = static_cast<Uint8>(mFadeAlpha);
        
        if (isSelected && mShowSelection)
        {
            // Usar cor amarela brilhante para seleção
            SDL_SetTextureColorMod(texture, 255, 255, 0);
        }
        else
        {
            // Usar branco puro para texto normal
            SDL_SetTextureColorMod(texture, 255, 255, 255);
        }

        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        
        // Resetar color mod
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    void GameOverScreen::handleInput(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    mSelectedOption = GameOverOption::RESTART;
                    break;
                
                case SDLK_s:
                case SDLK_DOWN:
                    mSelectedOption = GameOverOption::QUIT;
                    break;
                
                case SDLK_RETURN:
                case SDLK_SPACE:
                    mOptionConfirmed = true;
                    break;
            }
        }
    }

    void GameOverScreen::reset()
    {
        mSelectedOption = GameOverOption::RESTART;
        mOptionConfirmed = false;
        mFadeAlpha = 0.0f;
        mBlinkTimer = 0.0f;
        mShowSelection = true;
    }

    void GameOverScreen::startFadeIn()
    {
        mFadeAlpha = 0.0f;
    }
}
