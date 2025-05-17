#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "GameWorld.h"
#include "GameManager.h"

namespace BRTC {
    class Game {
    public:
        Game(SDL_Window* window, SDL_Renderer* renderer);
        ~Game();
        
        void handleEvents();
        void update();
        void render();
        bool isRunning();
        
    private:
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        
        GameWorld mWorld;
        GameManager mManager;
    };
}

#endif