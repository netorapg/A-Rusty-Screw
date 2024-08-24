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
    Game(SDL_Window* window, SDL_Renderer* renderer);
    void handleEvents();
    void update();
    void render();
    bool isQuit() const; // Mantém o método para verificar se o jogo deve ser encerrado

private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    bool mQuit; // Variável para controlar o estado de execução
    std::list<Platform> mPlatforms;
    std::list<SolidPlatform> mSolidPlatforms;
    std::list<Wall> mWalls;
    Player mPlayer;
};

#endif // GAME_H
