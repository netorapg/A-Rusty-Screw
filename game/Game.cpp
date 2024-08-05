#include "Game.h"
#include "../config.h"
#include <iostream>

Game::Game() : mWindow(nullptr), mRenderer(nullptr), mQuit(false), mPlayer(25, 5, mPlatforms, mSolidPlatforms, mWalls) {
    std::cout << "Game constructor called" << std::endl;
}

bool Game::init() {
    std::cout << "Initializing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Creating window" << std::endl;
    mWindow = SDL_CreateWindow("Platform 2d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (mWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Creating renderer" << std::endl;
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (mRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Adding platforms" << std::endl;
    mPlatforms.push_back(Platform(10, SCREEN_HEIGHT - 150, 200, 20));
    mSolidPlatforms.push_back(SolidPlatform(100, SCREEN_HEIGHT - 150, 570, 20));
    mWalls.push_back(Wall(200, SCREEN_HEIGHT - 250, 20,100));
    mSolidPlatforms.push_back(SolidPlatform(200,SCREEN_HEIGHT - 260, 20, 10));
    
    mWalls.push_back(Wall(0, 0, 20, 800));
    mWalls.push_back(Wall(620, 0, 20, 800));

    std::cout << "Initialization complete" << std::endl;
    return true;
}

void Game::close() {
    std::cout << "Closing game" << std::endl;
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            mQuit = true;
        }
        mPlayer.handleEvent(e);
    }
}

void Game::update() {
    mPlayer.move();
}

void Game::render() {
    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mRenderer);

    for (auto& platform : mPlatforms) {
        platform.render(mRenderer);
    }

    for (auto& solidPlatform : mSolidPlatforms) {
        solidPlatform.render(mRenderer);
    }

    for (auto &wall : mWalls){
        wall.render(mRenderer);
    }

    mPlayer.render(mRenderer);

    SDL_RenderPresent(mRenderer);
}

bool Game::isQuit() const {
    return mQuit;
}
