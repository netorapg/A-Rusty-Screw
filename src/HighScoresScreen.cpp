#include "../include/arscrew/HighScoresScreen.h"
#include <SDL2/SDL_ttf.h>

HighScoresScreen::HighScoresScreen(HighScoresManager& manager)
    : scoresManager(manager), done(false) {}

void HighScoresScreen::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && 
        (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE)) {
        done = true;
    }
}

void HighScoresScreen::update() {}

void HighScoresScreen::render(SDL_Renderer* renderer) {

}

bool HighScoresScreen::finished() const {
    return done;
}
