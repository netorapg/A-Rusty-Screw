#include "Game.h"
#include "../config.h"
#include <iostream>

Game::Game() : mWindow(nullptr), mRenderer(nullptr), mQuit(false), mPlayer(25, 5, mPlatforms, mSolidPlatforms, mWalls) {
    std::cout << "Game constructor called" << std::endl;
}

bool Game::init() {
    std::cout << "Initializing SDL" << std::endl;
    int result = SDL_Init(SDL_INIT_VIDEO);
if (result < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
} else {
    std::cout << "SDL initialized successfully." << std::endl;
}

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
    std::cout << "Handling events" << std::endl;
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        std::cout << "Event polled: " << e.type << std::endl;
        if (e.type == SDL_QUIT) {
            std::cout << "SDL_QUIT event received" << std::endl;
            mQuit = true;
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
    for (auto &wall : mWalls){
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