#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <SDL2/SDL.h


namespace BRTC
{
    class GameWorld
    {
        public:
            GameWorld(SDL_Window* window, SDL_Renderer* renderer);
            ~GameWorld();
            void handleEvents();
            void render();
            void update();
            void render(SDL_Renderer* renderer);

        private:
            SDL_Renderer* mRenderer

    }
}