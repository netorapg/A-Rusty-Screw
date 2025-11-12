#include "../include/arscrew/ScoreEntryScreen.h"
#include <algorithm>
#include <SDL2/SDL.h>
#include <iostream>

using namespace ARSCREW;

ScoreEntryScreen::ScoreEntryScreen(float score)
    : initials("___"), score(score), cursorPos(0), done(false), 
      titleFont(nullptr), textFont(nullptr), blinkTimer(0.0f), showCursor(true),
      fadeAlpha(0.0f), fadeSpeed(300.0f) {
    initializeFonts();
    calculatePositions();
}

void ScoreEntryScreen::handleEvent(const SDL_Event& e) {
    if (done) return;
    if (e.type == SDL_KEYDOWN) {
        char c = 0;
        if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) 
            c = 'A' + (e.key.keysym.sym - SDLK_a);
        else if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) 
            c = '0' + (e.key.keysym.sym - SDLK_0);
        else if (e.key.keysym.sym == SDLK_MINUS)
            c = '-';
        else if (e.key.keysym.sym == SDLK_SPACE)
            c = ' ';
        if (c && cursorPos < 3) {
            initials[cursorPos] = c;
            cursorPos++;
        }
        if (e.key.keysym.sym == SDLK_BACKSPACE && cursorPos > 0) {
            cursorPos--;
            initials[cursorPos] = '_';
        }
        if ((e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) && cursorPos == 3) {
            done = true;
        }
    }
}

void ScoreEntryScreen::update() {
    // Fade in effect
    if (fadeAlpha < 255.0f) {
        fadeAlpha += fadeSpeed * (1.0f/60.0f); // Assume 60 FPS
        if (fadeAlpha > 255.0f) fadeAlpha = 255.0f;
    }
    
    // Efeito de piscar do cursor
    blinkTimer += 1.0f/60.0f; // Assume 60 FPS
    if (blinkTimer >= 0.5f) {
        showCursor = !showCursor;
        blinkTimer = 0.0f;
    }
}

void ScoreEntryScreen::render(SDL_Renderer* renderer) {
    // Renderizar background escuro
    renderBackground(renderer);
    
    if (!titleFont || !textFont) return;
    
    // Só renderizar se o fade in começou
    if (fadeAlpha > 0.0f) {
        Uint8 alpha = static_cast<Uint8>(fadeAlpha);
        
        // Título "NEW HIGH SCORE!"
        SDL_Color titleColor = {255, 255, 0, alpha}; // Amarelo com fade
        renderText(renderer, "NEW HIGH SCORE!", titleFont, titleColor, 
                  positions.titleRect.x + positions.titleRect.w/2, positions.titleRect.y, true);
        
        // Score
        SDL_Color scoreColor = {255, 255, 255, alpha}; // Branco com fade
        std::string scoreText = "Score: " + std::to_string(static_cast<int>(score));
        renderText(renderer, scoreText, textFont, scoreColor, 
                  positions.scoreRect.x + positions.scoreRect.w/2, positions.scoreRect.y, true);
        
        // Instrução
        renderText(renderer, "Enter your initials:", textFont, scoreColor, 
                  positions.promptRect.x + positions.promptRect.w/2, positions.promptRect.y, true);
        
        // Initials com cursor
        std::string displayInitials = initials;
        if (showCursor && cursorPos < 3) {
            displayInitials[cursorPos] = '|';
        }
        
        SDL_Color initialsColor = {0, 255, 255, alpha}; // Cyan com fade
        renderText(renderer, displayInitials, titleFont, initialsColor, 
                  positions.initialsRect.x + positions.initialsRect.w/2, positions.initialsRect.y, true);
        
        // Instruções
        SDL_Color instructionColor = {180, 180, 180, alpha}; // Cinza com fade
        std::string instructionText;
        if (cursorPos < 3) {
            instructionText = "Type letters/numbers, BACKSPACE to delete";
        } else {
            instructionText = "Press ENTER to confirm";
        }
        renderText(renderer, instructionText, textFont, instructionColor, 
                  positions.instructionRect.x + positions.instructionRect.w/2, 
                  positions.instructionRect.y, true);
    }
}

bool ScoreEntryScreen::finished() const { return done; }
std::string ScoreEntryScreen::getInitials() const { return initials; }
float ScoreEntryScreen::getScore() const { return score; }

ScoreEntryScreen::~ScoreEntryScreen() {
    if (titleFont) TTF_CloseFont(titleFont);
    if (textFont) TTF_CloseFont(textFont);
}

void ScoreEntryScreen::initializeFonts() {
    const char* fontPaths[] = {
        "../assets/fonts/Megadeth.ttf",
        "../assets/fonts/METALORD.TTF", 
        "../assets/fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };
    
    for (const char* fontPath : fontPaths) {
        titleFont = TTF_OpenFont(fontPath, 48);
        if (titleFont) break;
    }
    
    for (const char* fontPath : fontPaths) {
        textFont = TTF_OpenFont(fontPath, 24);
        if (textFont) break;
    }
    
    if (!titleFont || !textFont) {
        std::cerr << "Failed to load fonts for ScoreEntryScreen!" << std::endl;
    }
}

void ScoreEntryScreen::renderText(SDL_Renderer* renderer, const std::string& text, TTF_Font* font,
                                 SDL_Color color, int x, int y, bool center) {
    if (!font) return;
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) return;
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }
    
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    
    SDL_Rect destRect = {
        center ? x - textWidth/2 : x,
        y,
        textWidth,
        textHeight
    };
    
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    SDL_DestroyTexture(textTexture);
}

void ScoreEntryScreen::calculatePositions() {
    // Título (centro superior, similar ao GameOverScreen)
    positions.titleRect = {
        SCREEN_WIDTH / 2 - 250,  // Largura estimada do título
        SCREEN_HEIGHT / 2 - 150, // Posição similar ao GameOver
        500, 60
    };
    
    // Score (abaixo do título)
    positions.scoreRect = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 80,
        300, 40
    };
    
    // Prompt "Enter your initials"
    positions.promptRect = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 30,
        300, 30
    };
    
    // Área das iniciais (centro da tela)
    positions.initialsRect = {
        SCREEN_WIDTH / 2 - 100,
        SCREEN_HEIGHT / 2 + 20,
        200, 60
    };
    
    // Instruções (parte inferior)
    positions.instructionRect = {
        SCREEN_WIDTH / 2 - 250,
        SCREEN_HEIGHT / 2 + 120,
        500, 30
    };
}

void ScoreEntryScreen::renderBackground(SDL_Renderer* renderer) {
    // Background escuro com fade, igual ao GameOverScreen
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(fadeAlpha * 0.8f));
    
    SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &fullScreen);
}
