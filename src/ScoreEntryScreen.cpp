#include "../include/arscrew/ScoreEntryScreen.h"
#include <algorithm>
#include <SDL2/SDL.h>

ScoreEntryScreen::ScoreEntryScreen(int score)
    : initials("___"), score(score), cursorPos(0), done(false) {}

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

void ScoreEntryScreen::update() {}

void ScoreEntryScreen::render(SDL_Renderer* renderer) {
    // Placeholder: Actual rendering code would go here
    // For example, render the initials and score on the screen
}

bool ScoreEntryScreen::finished() const { return done; }
std::string ScoreEntryScreen::getInitials() const { return initials; }
int ScoreEntryScreen::getScore() const { return score; }
