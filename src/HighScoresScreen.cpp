#include "../include/arscrew/HighScoresScreen.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "../include/arscrew/Globals.h"

using namespace ARSCREW;

HighScoresScreen::HighScoresScreen(HighScoresManager& manager)
    : scoresManager(manager), done(false), titleFont(nullptr), scoreFont(nullptr),
      fadeAlpha(0.0f), fadeSpeed(300.0f) {
    initializeFonts();
    calculatePositions();
}

void HighScoresScreen::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && 
        (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE)) {
        done = true;
    }
}

void HighScoresScreen::update() {
    // Fade in effect
    if (fadeAlpha < 255.0f) {
        fadeAlpha += fadeSpeed * (1.0f/60.0f); // Assume 60 FPS
        if (fadeAlpha > 255.0f) fadeAlpha = 255.0f;
    }
}

void HighScoresScreen::render(SDL_Renderer* renderer) {
    // Renderizar background escuro
    renderBackground(renderer);
    
    if (!titleFont || !scoreFont) return;
    
    // Só renderizar se o fade in começou
    if (fadeAlpha > 0.0f) {
        Uint8 alpha = static_cast<Uint8>(fadeAlpha);
        
        // Debug das posições calculadas
        static bool positionsDebug = true;
        if (positionsDebug) {
            std::cout << "=== DEBUG Posições HighScoresScreen ===" << std::endl;
            std::cout << "titleRect: x=" << positions.titleRect.x << " y=" << positions.titleRect.y 
                      << " w=" << positions.titleRect.w << " h=" << positions.titleRect.h << std::endl;
            std::cout << "scoresAreaRect: x=" << positions.scoresAreaRect.x << " y=" << positions.scoresAreaRect.y 
                      << " w=" << positions.scoresAreaRect.w << " h=" << positions.scoresAreaRect.h << std::endl;
            std::cout << "Alpha atual: " << static_cast<int>(alpha) << std::endl;
            positionsDebug = false;
        }

        // Título "HIGH SCORES"
        SDL_Color titleColor = {255, 255, 0, alpha}; // Amarelo com fade
        int titleX = positions.titleRect.x + positions.titleRect.w/2;
        int titleY = positions.titleRect.y;
        std::cout << "Renderizando título em x=" << titleX << " y=" << titleY << std::endl;
        renderText(renderer, "HIGH SCORES", titleFont, titleColor, titleX, titleY, true);
        
        // Renderizar lista de pontuações
        const auto& scores = scoresManager.getScores();
        SDL_Color scoreColor = {255, 255, 255, alpha}; // Branco com fade
        
        // Debug: verificar se os scores estão sendo carregados
        static bool firstRender = true;
        if (firstRender) {
            std::cout << "=== DEBUG HighScoresScreen ===" << std::endl;
            std::cout << "Número de scores carregados: " << scores.size() << std::endl;
            for (size_t i = 0; i < scores.size(); ++i) {
                std::cout << "Score " << i << ": " << scores[i].initials << " - " << scores[i].score << std::endl;
            }
            firstRender = false;
        }
        
        int startY = positions.scoresAreaRect.y;
        int lineHeight = 45;
        int centerX = SCREEN_WIDTH / 2;
        
        if (scores.empty()) {
            std::cout << "Scores estão vazios! Exibindo mensagem de 'No scores yet'" << std::endl;
            renderText(renderer, "No scores yet!", scoreFont, scoreColor, 
                      centerX, startY + 100, true);
        } else {
            for (size_t i = 0; i < scores.size() && i < 10; ++i) {
                // Formatação melhorada com alinhamento fixo
                char line[64];
                snprintf(line, sizeof(line), "%2zu. %-8s %8d", 
                        i + 1, 
                        scores[i].initials.c_str(), 
                        static_cast<int>(scores[i].score));
                
                int y = startY + (i * lineHeight);
                
                // Alternar cores para melhor legibilidade
                SDL_Color itemColor = scoreColor;
                if (i % 2 == 1) {
                    itemColor.r = static_cast<Uint8>(scoreColor.r * 0.9f);
                    itemColor.g = static_cast<Uint8>(scoreColor.g * 0.9f);
                    itemColor.b = static_cast<Uint8>(scoreColor.b * 0.9f);
                }
                
                renderText(renderer, std::string(line), scoreFont, itemColor, centerX, y, true);
            }
        }
        
        // Instruções
        SDL_Color instructionColor = {180, 180, 180, alpha}; // Cinza claro com fade
        renderText(renderer, "Press ENTER or ESC to return", scoreFont, instructionColor, 
                  positions.instructionRect.x + positions.instructionRect.w/2, 
                  positions.instructionRect.y, true);
    }
}

bool HighScoresScreen::finished() const {
    return done;
}

HighScoresScreen::~HighScoresScreen() {
    if (titleFont) TTF_CloseFont(titleFont);
    if (scoreFont) TTF_CloseFont(scoreFont);
}

void HighScoresScreen::initializeFonts() {
    const char* fontPaths[] = {
        "../assets/fonts/Megadeth.ttf",
        "../assets/fonts/METALORD.TTF", 
        "../assets/fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };
    
    std::cout << "=== Carregando fontes HighScoresScreen ===" << std::endl;
    
    for (const char* fontPath : fontPaths) {
        titleFont = TTF_OpenFont(fontPath, 48);
        if (titleFont) {
            std::cout << "HighScoresScreen: Title font carregada de " << fontPath << std::endl;
            break;
        } else {
            std::cout << "HighScoresScreen: Falha ao carregar title font " << fontPath << ": " << TTF_GetError() << std::endl;
        }
    }
    
    for (const char* fontPath : fontPaths) {
        scoreFont = TTF_OpenFont(fontPath, 24);
        if (scoreFont) {
            std::cout << "HighScoresScreen: Score font carregada de " << fontPath << std::endl;
            break;
        } else {
            std::cout << "HighScoresScreen: Falha ao carregar score font " << fontPath << ": " << TTF_GetError() << std::endl;
        }
    }
    
    if (!titleFont || !scoreFont) {
        std::cerr << "CRITICAL: Failed to load fonts for HighScoresScreen!" << std::endl;
        std::cout << "titleFont: " << (titleFont ? "OK" : "NULL") << std::endl;
        std::cout << "scoreFont: " << (scoreFont ? "OK" : "NULL") << std::endl;
    }
}

void HighScoresScreen::renderText(SDL_Renderer* renderer, const std::string& text, TTF_Font* font,
                                 SDL_Color color, int x, int y, bool center) {
    if (!font) {
        std::cout << "ERROR: Font é null em renderText!" << std::endl;
        return;
    }
    
    // std::cout << "renderText chamado: '" << text << "' em (" << x << "," << y << ") center=" << center << std::endl;
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) {
        std::cout << "ERROR: Falha ao criar textSurface: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cout << "ERROR: Falha ao criar textTexture: " << SDL_GetError() << std::endl;
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
    
    // std::cout << "Renderizando texto '" << text << "' no destRect: x=" << destRect.x 
    //          << " y=" << destRect.y << " w=" << destRect.w << " h=" << destRect.h << std::endl;
    
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    SDL_DestroyTexture(textTexture);
}

void HighScoresScreen::calculatePositions() {
    // Título (centro superior, similar ao StartMenu)
    positions.titleRect = {
        SCREEN_WIDTH / 2 - 200,  
        SCREEN_HEIGHT / 2 - 200, // Usar posicionamento relativo como StartMenu
        400, 60
    };
    
    // Área de scores (centro da tela)
    positions.scoresAreaRect = {
        SCREEN_WIDTH / 2 - 300,  
        SCREEN_HEIGHT / 2 - 100, // Começar no centro da tela
        600, 300                 // Espaço para scores
    };
    
    // Instruções (parte inferior, similar ao StartMenu)
    positions.instructionRect = {
        SCREEN_WIDTH / 2 - 200,
        SCREEN_HEIGHT / 2 + 220, // Usar posição relativa igual StartMenu
        400, 30
    };
}

void HighScoresScreen::renderBackground(SDL_Renderer* renderer) {
    // Background escuro com fade, igual ao GameOverScreen
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(fadeAlpha * 0.8f));
    
    SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &fullScreen);
}
