#pragma once
#include <SDL2/SDL.h>
#include "HighScoresManager.h"

class HighScoresScreen {
public:
    HighScoresScreen(HighScoresManager& manager);
    void handleEvent(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);
    bool finished() const;
private:
    HighScoresManager& scoresManager;
    bool done;
};