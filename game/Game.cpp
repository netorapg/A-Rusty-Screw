#include "Game.h"
#include "../config.h"
#include <iostream>

Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false),
      mPlayer(25, 5, mPlatforms, mSolidPlatforms, mWalls, mCrates, renderer) // Adicionei o 'renderer' aqui
{
    std::cout << "Game constructor called" << std::endl;

    // Inicializa o SDL_ttf
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Carrega a fonte grande
    mFont = TTF_OpenFont("../../platfom2d/assets/All Star Resort.ttf", 100); // Certifique-se que o caminho e a fonte estão corretos
    if (mFont == nullptr)
    {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Carregar a fonte menor
    mSmallFont = TTF_OpenFont("../../platfom2d/assets/Type Machine.ttf", 24); // Tamanho 24 para o texto menor
    if (mSmallFont == nullptr)
    {
        std::cerr << "Failed to load small font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Exemplo de adição de plataformas e paredes
    mSolidPlatforms.push_back(SolidPlatform(10, SCREEN_HEIGHT - 150, 570, 20));
    mSolidPlatforms.push_back(SolidPlatform(800, SCREEN_HEIGHT - 150, 570, 20));
    mPlatforms.push_back(Platform(500, SCREEN_HEIGHT - 340, 50, 20));
    mPlatforms.push_back(Platform(500, SCREEN_HEIGHT - 540, 50, 20));
    mSolidPlatforms.push_back(SolidPlatform(570, SCREEN_HEIGHT - 540, 220, 20));
    mSolidPlatforms.push_back(SolidPlatform(1200, SCREEN_HEIGHT - 420, 220, 20));
    mWalls.push_back(Wall(0, 0, 20, 800));
    mWalls.push_back(Wall(1260, 0, 20, 800));
    mCrates.push_back(Crate(700, SCREEN_HEIGHT - 590, 50, 50));
}

Game::~Game()
{
    // Libera as fontes e finaliza o TTF
    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
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
    mPlayer.move();
    for (auto &crate : mCrates)
        crate.update(mSolidPlatforms);
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // Branco
    SDL_RenderClear(mRenderer);

    // Renderizar plataformas, paredes, etc.
    for (auto &platform : mPlatforms)
        platform.render(mRenderer);

    for (auto &solidPlatform : mSolidPlatforms)
        solidPlatform.render(mRenderer);

    for (auto &wall : mWalls)
        wall.render(mRenderer);
    
    for (auto &crate : mCrates)
        crate.render(mRenderer);

    mPlayer.render(mRenderer);

    // Renderizar o texto "2D Lab"
    renderText("2D Lab", 50, 50, mFont); // Texto grande

    // Renderizar o texto "utilize W A S D para mover" com fonte menor
    renderText("utilize W A S D para mover", 50, 150, mSmallFont);
    renderText("utilize ESPACO para pular", 50, 200, mSmallFont);

    SDL_RenderPresent(mRenderer);
}

bool Game::isQuit() const
{
    return mQuit;
}

// Função auxiliar para renderizar texto
void Game::renderText(const char* text, int x, int y, TTF_Font* font)
{
    SDL_Color textColor = {0, 0, 0}; // Cor preta para o texto
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface == nullptr)
    {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
    if (textTexture == nullptr)
    {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h}; // Definir posição e tamanho do texto
    SDL_RenderCopy(mRenderer, textTexture, nullptr, &renderQuad); // Renderiza o texto

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
