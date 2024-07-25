#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Player.h"
#include "Platform.h"
#include <vector>

class Game
{
public:
    Game();

    bool init();
    void close();
    void handleEvents();
    void update();
    void render();
    bool isQuit() const;

private:
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player mPlayer;
    bool mQuit;
    std::vector<Platform> mPlatforms;
};

#endif // GAME_H