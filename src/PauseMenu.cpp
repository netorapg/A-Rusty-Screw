#include "../include/arscrew/PauseMenu.h"
#include <iostream>

namespace ARSCREW
{
    PauseMenu::PauseMenu(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mTitleFont(nullptr)
        , mOptionFont(nullptr)
        , mTitleTexture(nullptr)
        , mResumeTexture(nullptr)
        , mRestartTexture(nullptr)
        , mMainMenuTexture(nullptr)
        , mQuitTexture(nullptr)
        , mInstructionTexture(nullptr)
        , mSelectedOption(PauseMenuOption::RESUME)
        , mOptionConfirmed(false)
        , mVisible(false)
        , mBlinkTimer(0.0f)
        , mShowSelection(true)
    {
        initializeFonts();
        calculatePositions();
        createTextures();

        // Cores
        mTextColor = {255, 255, 255, 255};        // Branco puro
        mSelectedColor = {255, 255, 0, 255};      // Amarelo brilhante
        mBackgroundColor = {0, 0, 0, 180};        // Fundo escuro semitransparente
    }

    PauseMenu::~PauseMenu()
    {
        destroyTextures();
        if (mTitleFont) TTF_CloseFont(mTitleFont);
        if (mOptionFont) TTF_CloseFont(mOptionFont);
    }

    void PauseMenu::initializeFonts()
    {
        const char* fontPaths[] = {
            "../assets/fonts/METALORD.TTF",
            "../assets/fonts/Megadeth.ttf",
            "../assets/fonts/Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux fallback
            "/System/Library/Fonts/Arial.ttf",                  // macOS fallback
            "C:\\Windows\\Fonts\\arial.ttf"                     // Windows fallback
        };

        for (const char* fontPath : fontPaths)
        {
            mTitleFont = TTF_OpenFont(fontPath, 48); // Fonte para o título
            if (mTitleFont) {
                std::cout << "PauseMenu: Loaded title font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "PauseMenu: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        for (const char* fontPath : fontPaths)
        {
            mOptionFont = TTF_OpenFont(fontPath, 28); // Fonte para as opções
            if (mOptionFont) {
                std::cout << "PauseMenu: Loaded option font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "PauseMenu: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        if (!mTitleFont || !mOptionFont)
        {
            std::cerr << "CRITICAL: Failed to load any fonts for Pause Menu!" << std::endl;
        }
    }

    void PauseMenu::calculatePositions()
    {
        // Título PAUSED (centro superior)
        mPositions.titleRect = {
            SCREEN_WIDTH / 2 - 120,
            SCREEN_HEIGHT / 2 - 140,
            240, 60
        };

        // Opção Resume
        mPositions.resumeRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 - 60,
            200, 40
        };

        // Opção Restart
        mPositions.restartRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 - 10,
            200, 40
        };

        // Opção Main Menu
        mPositions.mainMenuRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 + 40,
            200, 40
        };

        // Opção Quit
        mPositions.quitRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 + 90,
            200, 40
        };

        // Instruções
        mPositions.instructionRect = {
            SCREEN_WIDTH / 2 - 180,
            SCREEN_HEIGHT / 2 + 150,
            360, 30
        };
    }

    void PauseMenu::createTextures()
    {
        if (!mTitleFont || !mOptionFont) {
            std::cerr << "PauseMenu: Cannot create textures - fonts not loaded!" << std::endl;
            return;
        }

        std::cout << "PauseMenu: Creating textures..." << std::endl;
        
        mTitleTexture = createTextTexture("PAUSED", mTitleFont, {255, 255, 255, 255}); // Branco
        mResumeTexture = createTextTexture("RESUME", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mRestartTexture = createTextTexture("RESTART", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mMainMenuTexture = createTextTexture("MAIN MENU", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mQuitTexture = createTextTexture("QUIT", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mInstructionTexture = createTextTexture("Use W/S to select, ENTER to confirm, ESC to resume", 
                                               mOptionFont, {220, 220, 220, 255}); // Cinza claro
        
        std::cout << "PauseMenu: Textures created - Title: " << (mTitleTexture ? "OK" : "FAIL") 
                  << ", Resume: " << (mResumeTexture ? "OK" : "FAIL")
                  << ", Restart: " << (mRestartTexture ? "OK" : "FAIL")
                  << ", MainMenu: " << (mMainMenuTexture ? "OK" : "FAIL")
                  << ", Quit: " << (mQuitTexture ? "OK" : "FAIL")
                  << ", Instructions: " << (mInstructionTexture ? "OK" : "FAIL") << std::endl;
    }

    void PauseMenu::destroyTextures()
    {
        if (mTitleTexture) SDL_DestroyTexture(mTitleTexture);
        if (mResumeTexture) SDL_DestroyTexture(mResumeTexture);
        if (mRestartTexture) SDL_DestroyTexture(mRestartTexture);
        if (mMainMenuTexture) SDL_DestroyTexture(mMainMenuTexture);
        if (mQuitTexture) SDL_DestroyTexture(mQuitTexture);
        if (mInstructionTexture) SDL_DestroyTexture(mInstructionTexture);

        mTitleTexture = nullptr;
        mResumeTexture = nullptr;
        mRestartTexture = nullptr;
        mMainMenuTexture = nullptr;
        mQuitTexture = nullptr;
        mInstructionTexture = nullptr;
    }

    SDL_Texture* PauseMenu::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color)
    {
        if (!font) {
            std::cerr << "PauseMenu: Cannot create texture - font is null!" << std::endl;
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "PauseMenu: Failed to create text surface for '" << text << "': " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        if (!texture) {
            std::cerr << "PauseMenu: Failed to create texture for '" << text << "': " << SDL_GetError() << std::endl;
        } else {
            std::cout << "PauseMenu: Successfully created texture for '" << text << "'" << std::endl;
        }

        return texture;
    }

    void PauseMenu::update(float deltaTime)
    {
        if (!mVisible) return;

        // Blink effect for selected option
        mBlinkTimer += deltaTime;
        if (mBlinkTimer >= 0.5f)
        {
            mShowSelection = !mShowSelection;
            mBlinkTimer = 0.0f;
        }
    }

    void PauseMenu::render(SDL_Renderer* renderer)
    {
        if (!mVisible) return;

        // Renderizar background escuro
        renderBackground(renderer);

        // Renderizar título
        if (mTitleTexture)
        {
            SDL_RenderCopy(renderer, mTitleTexture, nullptr, &mPositions.titleRect);
        }

        // Renderizar opções
        renderOption(renderer, mResumeTexture, mPositions.resumeRect, 
                    mSelectedOption == PauseMenuOption::RESUME);
        
        renderOption(renderer, mRestartTexture, mPositions.restartRect, 
                    mSelectedOption == PauseMenuOption::RESTART);
        
        renderOption(renderer, mMainMenuTexture, mPositions.mainMenuRect, 
                    mSelectedOption == PauseMenuOption::MAIN_MENU);
        
        renderOption(renderer, mQuitTexture, mPositions.quitRect, 
                    mSelectedOption == PauseMenuOption::QUIT);

        // Renderizar instruções
        if (mInstructionTexture)
        {
            SDL_RenderCopy(renderer, mInstructionTexture, nullptr, &mPositions.instructionRect);
        }
    }

    void PauseMenu::renderBackground(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, mBackgroundColor.r, mBackgroundColor.g, 
                              mBackgroundColor.b, mBackgroundColor.a);
        
        SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    void PauseMenu::renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
                                 const SDL_Rect& rect, bool isSelected)
    {
        if (!texture) return;
        
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

        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        
        // Resetar color mod
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    void PauseMenu::handleInput(const SDL_Event& event)
    {
        if (!mVisible) return;

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    // Navegar para cima
                    switch (mSelectedOption)
                    {
                        case PauseMenuOption::RESUME:
                            mSelectedOption = PauseMenuOption::QUIT; // Circular
                            break;
                        case PauseMenuOption::RESTART:
                            mSelectedOption = PauseMenuOption::RESUME;
                            break;
                        case PauseMenuOption::MAIN_MENU:
                            mSelectedOption = PauseMenuOption::RESTART;
                            break;
                        case PauseMenuOption::QUIT:
                            mSelectedOption = PauseMenuOption::MAIN_MENU;
                            break;
                    }
                    break;
                
                case SDLK_s:
                case SDLK_DOWN:
                    // Navegar para baixo
                    switch (mSelectedOption)
                    {
                        case PauseMenuOption::RESUME:
                            mSelectedOption = PauseMenuOption::RESTART;
                            break;
                        case PauseMenuOption::RESTART:
                            mSelectedOption = PauseMenuOption::MAIN_MENU;
                            break;
                        case PauseMenuOption::MAIN_MENU:
                            mSelectedOption = PauseMenuOption::QUIT;
                            break;
                        case PauseMenuOption::QUIT:
                            mSelectedOption = PauseMenuOption::RESUME; // Circular
                            break;
                    }
                    break;
                
                case SDLK_RETURN:
                case SDLK_SPACE:
                    mOptionConfirmed = true;
                    break;
                
                case SDLK_ESCAPE:
                    // ESC para resumir diretamente
                    mSelectedOption = PauseMenuOption::RESUME;
                    mOptionConfirmed = true;
                    break;
            }
        }
    }

    void PauseMenu::reset()
    {
        mSelectedOption = PauseMenuOption::RESUME;
        mOptionConfirmed = false;
        mBlinkTimer = 0.0f;
        mShowSelection = true;
    }

    void PauseMenu::show()
    {
        mVisible = true;
        reset();
    }

    void PauseMenu::hide()
    {
        mVisible = false;
    }
}
