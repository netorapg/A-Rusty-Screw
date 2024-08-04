#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <list>
#include "../player/Player.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"

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
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    bool mQuit;
    std::list<Platform> mPlatforms;
    std::list<SolidPlatform> mSolidPlatforms;
    std::list<Wall> mWalls;
    Player mPlayer;
};

#endif // GAME_H
