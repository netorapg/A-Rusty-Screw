#include <SDL2/SDL.h>
#include "player.h" 



class Game
{
public:
    Game() : mWindow(nullptr), mRenderer(nullptr), mQuit(false), mPlayer(0, 0) {}

    bool init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            return false;
        }

        mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (mWindow == nullptr)
        {
            return false;
        }

        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (mRenderer == nullptr)
        {
            return false;
        }

        return true;
    }

    void close()
    {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    void handleEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                mQuit = true;
            }
            mPlayer.handleEvent(e);
        }
    }

    void update()
    {
        mPlayer.move();
    }

    void render()
    {
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(mRenderer);

        mPlayer.render(mRenderer);

        SDL_RenderPresent(mRenderer);
    }

    bool isQuit() const { return mQuit; }

private:
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    Player mPlayer;
    bool mQuit;
  
};