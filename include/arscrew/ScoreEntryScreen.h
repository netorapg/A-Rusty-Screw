#pragma once
#include <string>
#include <SDL2/SDL.h>

class ScoreEntryScreen {
public:
    ScoreEntryScreen(int score);
    void handleEvent(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);
    bool finished() const;
    std::string getInitials() const;
    int getScore() const;
private:
    std::string initials;
    int score;
    int cursorPos;
    bool done;
};