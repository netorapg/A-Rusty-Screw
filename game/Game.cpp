#include "Game.h"
#include "../config.h"
#include <iostream>

Game::Game(SDL_Window* window, SDL_Renderer* renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false), 
      mPlayer(25, 5, mPlatforms, mSolidPlatforms, mWalls) {
    std::cout << "Game constructor called" << std::endl;
}

void Game::handleEvents() {
    std::cout << "Handling events" << std::endl;
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        std::cout << "Event polled: " << e.type << std::endl;
        if (e.type == SDL_QUIT) {
            std::cout << "SDL_QUIT event received" << std::endl;
            mQuit = true; // Atualize mQuit para true quando o evento SDL_QUIT for recebido
        }
        mPlayer.handleEvent(e);
    }
}

void Game::update() {
    std::cout << "Updating game state" << std::endl;
    mPlayer.move();
}

void Game::render() {
    std::cout << "Rendering game" << std::endl;
    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mRenderer);

    std::cout << "Rendering platforms" << std::endl;
    for (auto& platform : mPlatforms) {
        platform.render(mRenderer);
    }

    std::cout << "Rendering solid platforms" << std::endl;
    for (auto& solidPlatform : mSolidPlatforms) {
        solidPlatform.render(mRenderer);
    }

    std::cout << "Rendering walls" << std::endl;
    for (auto &wall : mWalls) {
        wall.render(mRenderer);
    }

    std::cout << "Rendering player" << std::endl;
    mPlayer.render(mRenderer);

    SDL_RenderPresent(mRenderer);
    std::cout << "Render complete" << std::endl;
}

bool Game::isQuit() const {
    return mQuit;
}
