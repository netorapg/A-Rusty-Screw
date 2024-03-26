#include "Game.h"
#include "config.h"

Game::Game() : mWindow(nullptr), mRenderer(nullptr), mQuit(false), mPlayer(0, 0) {}

bool Game::init()
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

    mPlatforms.push_back(Platform(100, SCREEN_HEIGHT - 50, 200, 20));
    mPlatforms.push_back(Platform(300, SCREEN_HEIGHT - 100, 200, 20));

    return true;
}

void Game::close()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::handleEvents()
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

void Game::update()
{
    // calcule o delta ao invés de salvar a posição anterior
    // limpe os .h e crie os .cpp
    // Aplicar a biblioteca de vetores do professor
    // tile map pesquisa
    mPlayer.move();
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mRenderer);

    for (auto &platform : mPlatforms)
    {
        platform.render(mRenderer);
    }

    mPlayer.render(mRenderer);

    SDL_RenderPresent(mRenderer);
}

bool Game::isQuit() const
{
    return mQuit;
}
