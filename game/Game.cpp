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

    mMusic = Mix_LoadMUS("../../platfom2d/assets/8-bit-game-158815.mp");
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

// Define a matriz do nível com quartos e aberturas entre eles
// Define a matriz do nível 10x15 com quartos e aberturas
int level[29][43] = {
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 3, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 3, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 3, 2, 2, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3,},
    {3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 3,},
    {3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 0, 3, 0, 3, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 3,},
    {3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 3,},
    {3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,},
    {3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,},
};



    // Mapear os números da matriz para os objetos do jogo
    const int tileSize = 30;  // Tamanho de cada tile no jogo
    for (int i = 0; i < 29; ++i)
    {
        for (int j = 0; j < 43; ++j)
        {
            int tileType = level[i][j];
            int x = j * tileSize;
            int y = i * tileSize;

            switch (tileType)
            {
            case 0: // Espaço vazio
                break;
            case 1: // SolidPlatform
                mSolidPlatforms.push_back(SolidPlatform(x, y, tileSize, tileSize));
                break;
            case 2: // Platform
                mPlatforms.push_back(Platform(x, y, tileSize, tileSize));
                break;
            case 3: // Parede
                mWalls.push_back(Wall(x, y, tileSize, tileSize));
                break;
            case 4: // Caixote (Crate)
                mCrates.push_back(Crate(x, y, 50, 50));
                break;
            }
        }
    }
}

Game::~Game()
{
   // Mix_FreeMusic(mMusic);
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

    for (auto &wall : mWalls)
        if (wall.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT))
            wall.render(mRenderer, mCamera.x, mCamera.y);

    for (auto &crate : mCrates)
        if (crate.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT))
            crate.render(mRenderer, mCamera.x, mCamera.y);

    // Renderizar o jogador
    mPlayer.render(mRenderer, mCamera.x, mCamera.y);

    SDL_RenderPresent(mRenderer);
}

bool Game::isRunning()
{
    return !mQuit;
}
