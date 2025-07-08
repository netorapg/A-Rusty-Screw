#include "../include/arscrew/CreditsScreen.h"
#include <iostream>

namespace ARSCREW
{
    CreditsScreen::CreditsScreen(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mTitleFont(nullptr)
        , mNormalFont(nullptr)
        , mSmallFont(nullptr)
        , mMainMenuTexture(nullptr)
        , mQuitTexture(nullptr)
        , mInstructionTexture(nullptr)
        , mSelectedOption(CreditsOption::MAIN_MENU)
        , mOptionConfirmed(false)
        , mShowOptions(false)
        , mScrollOffset(SCREEN_HEIGHT)
        , mScrollSpeed(30.0f)
        , mFadeAlpha(0.0f)
        , mFadeSpeed(100.0f)
        , mBlinkTimer(0.0f)
        , mShowSelection(true)
    {
        initializeFonts();
        createCreditLines();
        createOptionTextures();

        // Cores
        mTextColor = {255, 255, 255, 255};        // Branco puro
        mSelectedColor = {255, 255, 0, 255};      // Amarelo brilhante
        mBackgroundColor = {0, 0, 0, 255};        // Fundo preto
    }

    CreditsScreen::~CreditsScreen()
    {
        destroyTextures();
        if (mTitleFont) TTF_CloseFont(mTitleFont);
        if (mNormalFont) TTF_CloseFont(mNormalFont);
        if (mSmallFont) TTF_CloseFont(mSmallFont);
    }

    void CreditsScreen::initializeFonts()
    {
        const char* fontPaths[] = {
            "../assets/fonts/Megadeth.ttf",
            "../assets/fonts/METALORD.TTF",
            "../assets/fonts/Arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux fallback
            "/System/Library/Fonts/Arial.ttf",                  // macOS fallback
            "C:\\Windows\\Fonts\\arial.ttf"                     // Windows fallback
        };

        // Fonte para títulos
        for (const char* fontPath : fontPaths)
        {
            mTitleFont = TTF_OpenFont(fontPath, 48);
            if (mTitleFont) {
                std::cout << "CreditsScreen: Loaded title font from " << fontPath << std::endl;
                break;
            }
        }

        // Fonte normal
        for (const char* fontPath : fontPaths)
        {
            mNormalFont = TTF_OpenFont(fontPath, 24);
            if (mNormalFont) {
                std::cout << "CreditsScreen: Loaded normal font from " << fontPath << std::endl;
                break;
            }
        }

        // Fonte pequena
        for (const char* fontPath : fontPaths)
        {
            mSmallFont = TTF_OpenFont(fontPath, 18);
            if (mSmallFont) {
                std::cout << "CreditsScreen: Loaded small font from " << fontPath << std::endl;
                break;
            }
        }

        if (!mTitleFont || !mNormalFont || !mSmallFont)
        {
            std::cerr << "CRITICAL: Failed to load fonts for Credits Screen!" << std::endl;
        }
    }

    void CreditsScreen::createCreditLines()
    {
        mCreditLines.clear();

        // Título principal
        mCreditLines.push_back({"PARABENS!", 48, {255, 255, 0, 255}, 100.0f, nullptr});
        mCreditLines.push_back({"Voce derrotou o Punktauro!", 24, {255, 255, 255, 255}, 180.0f, nullptr});
        
        // Espaço
        mCreditLines.push_back({"", 24, {255, 255, 255, 255}, 260.0f, nullptr});
        
        // Créditos do jogo
        mCreditLines.push_back({"A RUSTY SCREW", 48, {255, 255, 255, 255}, 320.0f, nullptr});
        mCreditLines.push_back({"", 24, {255, 255, 255, 255}, 400.0f, nullptr});
        
        // Desenvolvedor
        mCreditLines.push_back({"DESENVOLVIDO POR", 24, {200, 200, 200, 255}, 460.0f, nullptr});
        mCreditLines.push_back({"Renato Augusto Platz G. Neto", 32, {255, 255, 255, 255}, 520.0f, nullptr});
        mCreditLines.push_back({"", 24, {255, 255, 255, 255}, 580.0f, nullptr});
        
        // Tecnologias
        mCreditLines.push_back({"FEITO COM", 24, {200, 200, 200, 255}, 640.0f, nullptr});
        mCreditLines.push_back({"C++ & SDL2", 24, {255, 255, 255, 255}, 700.0f, nullptr});
        mCreditLines.push_back({"", 24, {255, 255, 255, 255}, 760.0f, nullptr});
        
        // Agradecimentos
        mCreditLines.push_back({"AGRADECIMENTOS ESPECIAIS", 24, {200, 200, 200, 255}, 820.0f, nullptr});
        mCreditLines.push_back({"Professor Eduardo Henrique Molina da Cruz", 18, {255, 255, 255, 255}, 880.0f, nullptr});
        mCreditLines.push_back({"Carla, minha amada <3", 18, {255, 255, 255, 255}, 920.0f, nullptr});
        mCreditLines.push_back({"Tatiana, Minha mamae <3", 18, {255, 255, 255, 255}, 960.0f, nullptr});
        mCreditLines.push_back({"E a voce, por jogar!", 18, {255, 255, 255, 255}, 1030.0f, nullptr});
        mCreditLines.push_back({"", 24, {255, 255, 255, 255}, 1020.0f, nullptr});
        
        // Final
        mCreditLines.push_back({"OBRIGADO POR JOGAR!", 32, {255, 255, 0, 255}, 1100.0f, nullptr});

        // Criar texturas para cada linha
        for (auto& line : mCreditLines)
        {
            if (line.text.empty()) continue;
            
            TTF_Font* font = mSmallFont;
            if (line.fontSize >= 48) font = mTitleFont;
            else if (line.fontSize >= 24) font = mNormalFont;
            
            line.texture = createTextTexture(line.text, font, line.color);
        }
    }

    void CreditsScreen::createOptionTextures()
    {
        if (!mNormalFont) return;

        mMainMenuTexture = createTextTexture("MENU PRINCIPAL", mNormalFont, {255, 255, 255, 255});
        mQuitTexture = createTextTexture("SAIR", mNormalFont, {255, 255, 255, 255});
        mInstructionTexture = createTextTexture("Use W/S para navegar, ENTER para confirmar, SPACE para pular", 
                                               mSmallFont, {200, 200, 200, 255});
    }

    void CreditsScreen::destroyTextures()
    {
        for (auto& line : mCreditLines)
        {
            if (line.texture) SDL_DestroyTexture(line.texture);
        }
        mCreditLines.clear();

        if (mMainMenuTexture) SDL_DestroyTexture(mMainMenuTexture);
        if (mQuitTexture) SDL_DestroyTexture(mQuitTexture);
        if (mInstructionTexture) SDL_DestroyTexture(mInstructionTexture);

        mMainMenuTexture = nullptr;
        mQuitTexture = nullptr;
        mInstructionTexture = nullptr;
    }

    SDL_Texture* CreditsScreen::createTextTexture(const std::string& text, TTF_Font* font, SDL_Color color)
    {
        if (!font) {
            std::cerr << "CreditsScreen: Cannot create texture - font is null!" << std::endl;
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!textSurface) {
            std::cerr << "CreditsScreen: Failed to create text surface for '" << text << "': " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        if (!texture) {
            std::cerr << "CreditsScreen: Failed to create texture for '" << text << "': " << SDL_GetError() << std::endl;
        }

        return texture;
    }

    void CreditsScreen::update(float deltaTime)
    {
        if (!mShowOptions)
        {
            // Fade in
            if (mFadeAlpha < 255.0f)
            {
                mFadeAlpha += mFadeSpeed * deltaTime;
                if (mFadeAlpha > 255.0f) mFadeAlpha = 255.0f;
            }

            // Scroll dos créditos
            mScrollOffset -= mScrollSpeed * deltaTime;
            
            // Verificar se terminou de rolar
            if (mScrollOffset < -1200.0f) // Todos os créditos passaram
            {
                mShowOptions = true;
                mScrollOffset = -1200.0f; // Parar o scroll
            }
        }
        else
        {
            // Blink effect para as opções
            mBlinkTimer += deltaTime;
            if (mBlinkTimer >= 0.5f)
            {
                mShowSelection = !mShowSelection;
                mBlinkTimer = 0.0f;
            }
        }
    }

    void CreditsScreen::render(SDL_Renderer* renderer)
    {
        // Renderizar background
        renderBackground(renderer);

        Uint8 alpha = static_cast<Uint8>(mFadeAlpha);

        // Renderizar créditos rolando
        renderCredits(renderer);

        // Renderizar opções se terminaram de rolar
        if (mShowOptions)
        {
            renderOptions(renderer);
        }
    }

    void CreditsScreen::renderBackground(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, mBackgroundColor.r, mBackgroundColor.g, 
                              mBackgroundColor.b, mBackgroundColor.a);
        
        SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullScreen);
    }

    void CreditsScreen::renderCredits(SDL_Renderer* renderer)
    {
        Uint8 alpha = static_cast<Uint8>(mFadeAlpha);

        for (const auto& line : mCreditLines)
        {
            if (!line.texture) continue;

            int textWidth, textHeight;
            SDL_QueryTexture(line.texture, nullptr, nullptr, &textWidth, &textHeight);

            float yPos = mScrollOffset + line.yOffset;
            
            // Só renderizar se estiver visível na tela
            if (yPos >= -textHeight && yPos <= SCREEN_HEIGHT)
            {
                SDL_Rect destRect = {
                    (SCREEN_WIDTH - textWidth) / 2,
                    static_cast<int>(yPos),
                    textWidth,
                    textHeight
                };

                SDL_SetTextureAlphaMod(line.texture, alpha);
                SDL_RenderCopy(renderer, line.texture, nullptr, &destRect);
            }
        }
    }

    void CreditsScreen::renderOptions(SDL_Renderer* renderer)
    {
        // Posições das opções
        SDL_Rect mainMenuRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 + 50,
            200, 40
        };

        SDL_Rect quitRect = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 + 110,
            200, 40
        };

        SDL_Rect instructionRect = {
            SCREEN_WIDTH / 2 - 200,
            SCREEN_HEIGHT / 2 + 180,
            400, 30
        };

        // Renderizar opções
        renderOption(renderer, mMainMenuTexture, mainMenuRect, 
                    mSelectedOption == CreditsOption::MAIN_MENU);
        
        renderOption(renderer, mQuitTexture, quitRect, 
                    mSelectedOption == CreditsOption::QUIT);

        // Renderizar instruções
        if (mInstructionTexture)
        {
            SDL_SetTextureAlphaMod(mInstructionTexture, 255);
            SDL_RenderCopy(renderer, mInstructionTexture, nullptr, &instructionRect);
        }
    }

    void CreditsScreen::renderOption(SDL_Renderer* renderer, SDL_Texture* texture, 
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

        SDL_SetTextureAlphaMod(texture, 255);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        
        // Resetar color mod
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    void CreditsScreen::handleInput(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_SPACE:
                    // Pular para o final dos créditos
                    if (!mShowOptions)
                    {
                        skipToEnd();
                    }
                    break;

                case SDLK_w:
                case SDLK_UP:
                    if (mShowOptions)
                    {
                        mSelectedOption = (mSelectedOption == CreditsOption::MAIN_MENU) ? 
                                         CreditsOption::QUIT : CreditsOption::MAIN_MENU;
                    }
                    break;
                
                case SDLK_s:
                case SDLK_DOWN:
                    if (mShowOptions)
                    {
                        mSelectedOption = (mSelectedOption == CreditsOption::MAIN_MENU) ? 
                                         CreditsOption::QUIT : CreditsOption::MAIN_MENU;
                    }
                    break;
                
                case SDLK_RETURN:
                    if (mShowOptions)
                    {
                        mOptionConfirmed = true;
                    }
                    break;
            }
        }
    }

    void CreditsScreen::reset()
    {
        mSelectedOption = CreditsOption::MAIN_MENU;
        mOptionConfirmed = false;
        mShowOptions = false;
        mScrollOffset = SCREEN_HEIGHT;
        mFadeAlpha = 0.0f;
        mBlinkTimer = 0.0f;
        mShowSelection = true;
    }

    void CreditsScreen::startCredits()
    {
        reset();
    }

    void CreditsScreen::skipToEnd()
    {
        mShowOptions = true;
        mScrollOffset = -1200.0f;
        mFadeAlpha = 255.0f;
    }
}
