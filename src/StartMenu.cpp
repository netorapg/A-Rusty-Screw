#include "../include/arscrew/StartMenu.h"
#include <iostream>

namespace ARSCREW
{
    StartMenu::StartMenu(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mTitleFont(nullptr)
        , mOptionFont(nullptr)
        , mTitleTexture(nullptr)
        , mStartTexture(nullptr)
        , mQuitTexture(nullptr)
        , mInstructionTexture(nullptr)
        , mSelectedOption(StartMenuOption::START_GAME)
        , mOptionConfirmed(false)
        , mFadeAlpha(255.0f) // Começa visível
        , mFadeSpeed(300.0f)
        , mBlinkTimer(0.0f)
        , mShowSelection(true)
    {
        initializeFonts();
        calculatePositions();
        createTextures();

        // Cores
        mTextColor = {255, 255, 255, 255};        // Branco puro
        mSelectedColor = {255, 255, 0, 255};      // Amarelo brilhante
        mBackgroundColor = {0, 0, 0, 200};        // Fundo escuro semitransparente
    }

    StartMenu::~StartMenu()
    {
        destroyTextures();
        if (mTitleFont) TTF_CloseFont(mTitleFont);
        if (mOptionFont) TTF_CloseFont(mOptionFont);
    }

    void StartMenu::initializeFonts()
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
            mTitleFont = TTF_OpenFont(fontPath, 64); // Fonte maior para o título
            if (mTitleFont) {
                std::cout << "StartMenu: Loaded title font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "StartMenu: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        for (const char* fontPath : fontPaths)
        {
            mOptionFont = TTF_OpenFont(fontPath, 32); // Fonte maior para as opções
            if (mOptionFont) {
                std::cout << "StartMenu: Loaded option font from " << fontPath << std::endl;
                break;
            } else {
                std::cout << "StartMenu: Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
            }
        }

        if (!mTitleFont || !mOptionFont)
        {
            std::cerr << "CRITICAL: Failed to load any fonts for Start Menu!" << std::endl;
        }
    }

    void StartMenu::calculatePositions()
    {
        // Título do jogo (centro superior)
        mPositions.titleRect = {
            SCREEN_WIDTH / 2 - 200,
            SCREEN_HEIGHT / 2 - 150,
            400, 80
        };

        // Opção Start Game
        mPositions.startRect = {
            SCREEN_WIDTH / 2 - 120,
            SCREEN_HEIGHT / 2 - 20,
            240, 50
        };

        // Opção Quit
        mPositions.quitRect = {
            SCREEN_WIDTH / 2 - 120,
            SCREEN_HEIGHT / 2 + 60,
            240, 50
        };

        // Instruções
        mPositions.instructionRect = {
            SCREEN_WIDTH / 2 - 200,
            SCREEN_HEIGHT / 2 + 160,
            400, 40
        };
    }

    void StartMenu::createTextures()
    {
        if (!mTitleFont || !mOptionFont) {
            std::cerr << "StartMenu: Cannot create textures - fonts not loaded!" << std::endl;
            return;
        }

        std::cout << "StartMenu: Creating textures..." << std::endl;
        
        mTitleTexture = createTextTexture("A RUSTY SCREW", mTitleFont, {255, 255, 255, 255}); // Branco
        mStartTexture = createTextTexture("START GAME", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mQuitTexture = createTextTexture("QUIT", mOptionFont, {255, 255, 255, 255}); // Branco puro
        mInstructionTexture = createTextTexture("Use W/S to select, ENTER to confirm", 
                                               mOptionFont, {220, 220, 220, 255}); // Cinza claro
        
        std::cout << "StartMenu: Textures created - Title: " << (mTitleTexture ? "OK" : "FAIL") 
                  << ", Start: " << (mStartTexture ? "OK" : "FAIL")
                  << ", Quit: " << (mQuitTexture ? "OK" : "FAIL")
                  << ", Instructions: " << (mInstructionTexture ? "OK" : "FAIL") << std::endl;
    }

    void StartMenu::destroyTextures()
    {
        if (mTitleTexture) SDL_DestroyTexture(mTitleTexture);
        if (mStartTexture) SDL_DestroyTexture(mStartTexture);
        if (mQuitTexture) SDL_DestroyTexture(mQuitTexture);
        if (mInstructionTexture) SDL_DestroyTexture(mInstructionTexture);

        mTitleTexture = nullptr;
        mStartTexture = nullptr;
        mQuitTexture = nullptr;
        mInstructionTexture = nullptr;
    }

    SDL_Texture* StartMenu::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color)
    {
        if (!font) {
            std::cerr << "StartMenu: Cannot create texture - font is null!" << std::endl;
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "StartMenu: Failed to create text surface for '" << text << "': " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        if (!texture) {
            std::cerr << "StartMenu: Failed to create texture for '" << text << "': " << SDL_GetError() << std::endl;
        } else {
            std::cout << "StartMenu: Successfully created texture for '" << text << "'" << std::endl;
        }

        return texture;
    }

    void StartMenu::update(float deltaTime)
    {
        // Blink effect for selected option
        mBlinkTimer += deltaTime;
        if (mBlinkTimer >= 0.5f)
        {
            mShowSelection = !mShowSelection;
            mBlinkTimer = 0.0f;
        }
    }

    void StartMenu::render(SDL_Renderer* renderer)
    {
        // Renderizar background escuro
        renderBackground(renderer);

        Uint8 alpha = static_cast<Uint8>(mFadeAlpha);

        // Renderizar título do jogo
        if (mTitleTexture)
        {
            SDL_SetTextureAlphaMod(mTitleTexture, alpha);
            SDL_RenderCopy(renderer, mTitleTexture, nullptr, &mPositions.titleRect);
        }

        // Renderizar opções
        renderOption(renderer, mStartTexture, mPositions.startRect, 
                    mSelectedOption == StartMenuOption::START_GAME);
        
        renderOption(renderer, mQuitTexture, mPositions.quitRect, 
                    mSelectedOption == StartMenuOption::QUIT);

        // Renderizar instruções
        if (mInstructionTexture)
        {
            SDL_SetTextureAlphaMod(mInstructionTexture, alpha);
            SDL_RenderCopy(renderer, mInstructionTexture, nullptr, &mPositions.instructionRect);
        }
    }

    void StartMenu::renderBackground(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, mBackgroundColor.r, mBackgroundColor.g, 
                              mBackgroundColor.b, static_cast<Uint8>(mFadeAlpha * 0.8f));
        
        SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    void StartMenu::renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
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

    void StartMenu::handleInput(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    mSelectedOption = StartMenuOption::START_GAME;
                    break;
                
                case SDLK_s:
                case SDLK_DOWN:
                    mSelectedOption = StartMenuOption::QUIT;
                    break;
                
                case SDLK_RETURN:
                case SDLK_SPACE:
                    mOptionConfirmed = true;
                    break;
            }
        }
    }

    void StartMenu::reset()
    {
        mSelectedOption = StartMenuOption::START_GAME;
        mOptionConfirmed = false;
        mFadeAlpha = 255.0f;
        mBlinkTimer = 0.0f;
        mShowSelection = true;
    }

    void StartMenu::startFadeIn()
    {
        mFadeAlpha = 255.0f; // Já começa visível
    }
}
