#ifndef ARSCREW_SCOREENTRYSCREEN_H
#define ARSCREW_SCOREENTRYSCREEN_H
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Globals.h"

struct ScoreEntryPositions {
    SDL_Rect titleRect;
    SDL_Rect scoreRect;
    SDL_Rect promptRect;
    SDL_Rect initialsRect;
    SDL_Rect instructionRect;
};

class ScoreEntryScreen {
public:
    ScoreEntryScreen(float score);
    ~ScoreEntryScreen();
    void handleEvent(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);
    bool finished() const;
    std::string getInitials() const;
    float getScore() const;
private:
    std::string initials;
    float score;
    int cursorPos;
    bool done;
    TTF_Font* titleFont;
    TTF_Font* textFont;
    float blinkTimer;
    bool showCursor;
    float fadeAlpha;
    float fadeSpeed;
    ScoreEntryPositions positions;
    
    void initializeFonts();
    void calculatePositions();
    void renderText(SDL_Renderer* renderer, const std::string& text, TTF_Font* font,
                   SDL_Color color, int x, int y, bool center = false);
    void renderBackground(SDL_Renderer* renderer);
};
#endif // ARSCREW_SCOREENTRYSCREEN_H