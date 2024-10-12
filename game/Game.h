#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <list>
#include <SDL2/SDL_ttf.h>  // Incluir SDL_ttf para renderizar texto
#include "../player/Player.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"

class Game
{
public:
    Game(SDL_Window* window, SDL_Renderer* renderer);
    ~Game();  // Destrutor para liberar os recursos
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

    TTF_Font* mFont;       // Fonte grande
    TTF_Font* mSmallFont;  // Fonte menor para o texto "utilize W A S D para mover"
    void renderText(const char* text, int x, int y, TTF_Font* font); // Método para renderizar o texto
};

#endif // GAME_H