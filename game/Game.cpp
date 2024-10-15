#include "Game.h"
#include "../config.h"
#include <iostream>

Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false),
      mPlayer(25, 5, mPlatforms, mSolidPlatforms, mWalls, mCrates, renderer)
{
    std::cout << "Game constructor called" << std::endl;

    // Inicializar a câmera
    mCamera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

   SDL_Surface* loadedBackground = IMG_Load("/home/netorapg/projects/platfom2d/assets/background.png");
if (loadedBackground == nullptr)
{
    std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
}
mBackgroundTexture = SDL_CreateTextureFromSurface(renderer, loadedBackground);
SDL_FreeSurface(loadedBackground);

    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    mMusic = Mix_LoadMUS("../../platfom2d/assets/8-bit-game-158815.mp3");
    if (mMusic == nullptr)
    {
        std::cerr << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    mJumpSound = Mix_LoadWAV("../../platfom2d/assets/mixkit-player-jumping-in-a-video-game-2043.wav");
    if (mJumpSound == nullptr)
    {
        std::cerr << "Failed to load jump sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    Mix_PlayMusic(mMusic, -1);

    mFont = TTF_OpenFont("../../platfom2d/assets/All Star Resort.ttf", 100);
    if (mFont == nullptr)
    {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    mSmallFont = TTF_OpenFont("../../platfom2d/assets/Type Machine.ttf", 24);
    if (mSmallFont == nullptr)
    {
        std::cerr << "Failed to load small font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

 // Adicionando plataformas e outros elementos

// Plataforma base
mSolidPlatforms.push_back(SolidPlatform(0, SCREEN_HEIGHT - 50, 1280, 20)); // Plataforma de chão

// Crate
mCrates.push_back(Crate(200, SCREEN_HEIGHT - 100, 50, 50)); // Crate posicionado à esquerda

// Plataformas flutuantes
mPlatforms.push_back(Platform(300, SCREEN_HEIGHT - 200, 100, 20)); // Plataforma superior à direita
mPlatforms.push_back(Platform(500, SCREEN_HEIGHT - 300, 150, 20)); // Plataforma do meio
mPlatforms.push_back(Platform(800, SCREEN_HEIGHT - 250, 100, 20)); // Plataforma direita
mPlatforms.push_back(Platform(600, SCREEN_HEIGHT - 450, 150, 20)); // Plataforma alta no centro
mPlatforms.push_back(Platform(1000, SCREEN_HEIGHT - 350, 100, 20)); // Plataforma alta à direita
mPlatforms.push_back(Platform(1100, SCREEN_HEIGHT - 550, 150, 20)); // Plataforma superior à direita

// Paredes laterais
mWalls.push_back(Wall(0, 0, 20, SCREEN_HEIGHT)); // Parede esquerda
mWalls.push_back(Wall(1260, 0, 20, SCREEN_HEIGHT)); // Parede direita

// Obstáculos adicionais
mSolidPlatforms.push_back(SolidPlatform(300, SCREEN_HEIGHT - 400, 250, 20)); // Plataforma flutuante alta
mSolidPlatforms.push_back(SolidPlatform(700, SCREEN_HEIGHT - 350, 20, 150)); // Parede alta à direita
mSolidPlatforms.push_back(SolidPlatform(1000, SCREEN_HEIGHT - 600, 300, 20)); // Plataforma alta central
mSolidPlatforms.push_back(SolidPlatform(1200, SCREEN_HEIGHT - 250, 20, 300)); // Parede direita central

// Área de exploração
mSolidPlatforms.push_back(SolidPlatform(500, SCREEN_HEIGHT - 650, 300, 20)); // Plataforma elevada
mPlatforms.push_back(Platform(200, SCREEN_HEIGHT - 500, 50, 20)); // Plataforma flutuante à esquerda
mPlatforms.push_back(Platform(800, SCREEN_HEIGHT - 550, 50, 20)); // Plataforma flutuante à direita
mPlatforms.push_back(Platform(400, SCREEN_HEIGHT - 300, 100, 20)); // Plataforma flutuante no meio

}

Game::~Game()
{
    Mix_FreeMusic(mMusic);
    Mix_FreeChunk(mJumpSound);

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

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
        {
            Mix_PlayChannel(-1, mJumpSound, 0);
        }
    }
}

void Game::update()
{
    mPlayer.move();

    // Ajusta a posição da câmera para seguir o jogador
    mCamera.x = static_cast<int>(mPlayer.getPosX() - mCamera.w / 2);
    mCamera.y = static_cast<int>(mPlayer.getPosY() - mCamera.h / 2);

    // Limita a câmera dentro dos limites do cenário
    if (mCamera.x < 0)
    {
        mCamera.x = 0;
    }
    if (mCamera.y < 0)
    {
        mCamera.y = 0;
    }
    if (mCamera.x > LEVEL_WIDTH - mCamera.w)
    {
        mCamera.x = LEVEL_WIDTH - mCamera.w;
    }
    if (mCamera.y > LEVEL_HEIGHT - mCamera.h)
    {
        mCamera.y = LEVEL_HEIGHT - mCamera.h;
    }

    // Atualiza as caixas
    for (auto &crate : mCrates)
    {
        crate.update(mSolidPlatforms);
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // Branco
    SDL_RenderClear(mRenderer);

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &bgRect);


    // Renderizar plataformas, paredes, etc., somente se estiverem visíveis
    for (auto &platform : mPlatforms)
        if (platform.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT))
            platform.render(mRenderer, mCamera.x, mCamera.y);

    for (auto &solidPlatform : mSolidPlatforms)
        if (solidPlatform.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT))
            solidPlatform.render(mRenderer, mCamera.x, mCamera.y);

    // Renderizar as paredes e caixas como necessário
    for (auto &wall : mWalls)
        wall.render(mRenderer, mCamera.x, mCamera.y);

    for (auto &crate : mCrates)
        crate.render(mRenderer, mCamera.x, mCamera.y);

    mPlayer.render(mRenderer, mCamera.x, mCamera.y); // Passar a posição da câmera para o jogador

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

void Game::renderText(const char *text, int x, int y, TTF_Font *font)
{
    SDL_Color textColor = {0, 0, 0}; // Cor preta para o texto
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface == nullptr)
    {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
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