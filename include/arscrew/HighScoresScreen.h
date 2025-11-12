#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "HighScoresManager.h"
#include "Globals.h"

struct HighScoresPositions {
    SDL_Rect titleRect;
    SDL_Rect scoresAreaRect;
    SDL_Rect instructionRect;
};

class HighScoresScreen {
public:
    HighScoresScreen(HighScoresManager& manager);
    ~HighScoresScreen();
    void handleEvent(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);
    bool finished() const;
private:
    HighScoresManager& scoresManager;
    bool done;
    TTF_Font* titleFont;
    TTF_Font* scoreFont;
    HighScoresPositions positions;
    float fadeAlpha;
    float fadeSpeed;
    
    void initializeFonts();
    void calculatePositions();
    void renderText(SDL_Renderer* renderer, const std::string& text, TTF_Font* font, 
                   SDL_Color color, int x, int y, bool center = false);
    void renderBackground(SDL_Renderer* renderer);
};