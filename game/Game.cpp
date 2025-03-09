#include "Game.h"
#include "../config.h"
#include <iostream>
#include <unordered_map>

Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false),
      mPlayer(50, 5, renderer)
{
    std::cout << "Game constructor called" << std::endl;

    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    if(SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) {
        std::cerr << "Failed to set fullscreen mode: " << SDL_GetError() << std::endl;
    }

    // Inicializar a câmera
    mCamera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_Surface* loadedBackground = IMG_Load("/home/netorapg/projects/platfom2d/assets/166722.png");
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

loadLevelFromJSON("../map/level1.json");
}

Game::~Game()
{
   // Mix_FreeMusic(mMusic);
    Mix_FreeChunk(mJumpSound);

    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
}

void Game::loadLevelFromJSON(const std::string& filePath) {
    // Carregar o JSON
    json_object *root = json_object_from_file(filePath.c_str());
    if (!root) {
        std::cerr << "Failed to load JSON file: " << filePath << std::endl;
        return;
    }

    // Acessar o objeto "tiles" para mapear variáveis
    json_object *tiles;
    if (!json_object_object_get_ex(root, "tiles", &tiles)) {
        std::cerr << "Invalid JSON: missing 'tiles'" << std::endl;
        json_object_put(root);
        return;
    }

    // Criar um mapa para armazenar os valores das variáveis
    std::unordered_map<std::string, int> tileMap;

    // Iterar sobre as entradas do objeto "tiles"
    json_object_object_foreach(tiles, key, val) {
        tileMap[key] = json_object_get_int(val);
    }

    // Acessar o array "layers"
    json_object *layers;
    if (!json_object_object_get_ex(root, "layers", &layers)) {
        std::cerr << "Invalid JSON: missing 'layers'" << std::endl;
        json_object_put(root);
        return;
    }

    // Acessar o primeiro objeto dentro de "layers"
    json_object *first_layer = json_object_array_get_idx(layers, 0);
    if (!first_layer) {
        std::cerr << "Invalid JSON: no layers found" << std::endl;
        json_object_put(root);
        return;
    }

    // Acessar o array "data"
    json_object *data;
    if (!json_object_object_get_ex(first_layer, "data", &data)) {
        std::cerr << "Invalid JSON: missing 'data'" << std::endl;
        json_object_put(root);
        return;
    }

    // Tamanho de cada tile
    const int tileSize = 30;

    // Iterar sobre as linhas da matriz
    const int rows = json_object_array_length(data);
    for (int i = 0; i < rows; ++i) {
        json_object *row = json_object_array_get_idx(data, i);
        const int cols = json_object_array_length(row);

        for (int j = 0; j < cols; ++j) {
            json_object *tile = json_object_array_get_idx(row, j);
            const char *tileName = json_object_get_string(tile);

            // Verificar se o tileName existe no mapa
            if (tileMap.find(tileName) == tileMap.end()) {
                std::cerr << "Unknown tile name: " << tileName << std::endl;
                continue;
            }

            int tileType = tileMap[tileName];
            int x = j * tileSize;
            int y = i * tileSize;

            switch (tileType) {
                case 0: // Espaço vazio
                    break;
                case 1: // Plataforma
                    mPlatforms.emplace_back(x, y, tileSize, tileSize);
                    break;
                case 2: // Plataforma sólida
                    mSolidPlatforms.emplace_back(x, y, tileSize, tileSize);
                    break;
                case 3: // Parede
                    mWalls.emplace_back(x, y, tileSize, tileSize);
                    break;
                case 4: // Caixote
                    mCrates.emplace_back(x, y, 50, 50);
                    break;
                default:
                    std::cerr << "Unknown tile type: " << tileType << std::endl;
            }
        }
    }

    // Liberar a memória do JSON
    json_object_put(root);
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

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
        {
            resetGame();
        }

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
        {
            Mix_PlayChannel(-1, mJumpSound, 0);
        }

        // Alternar entre modo janela e tela cheia ao pressionar F11
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
            Uint32 flags = SDL_GetWindowFlags(mWindow);
            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                SDL_SetWindowFullscreen(mWindow, 0); // Voltar ao modo janela
            } else {
                SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP); // Entrar em tela cheia
            }
        }
    }
}
void Game::update()
{
    mPlayer.move();
    PhysicsEngine::HandleCollisions(mPlayer, mPlatforms, mSolidPlatforms, mWalls, mCrates);

    if (!mPlayer.isOnGround())
    {
        mPlayer.setFalling(true);
    }

    std::cout << "Player Position: (" << mPlayer.getPosX() << ", " << mPlayer.getPosY() << ")\n";
std::cout << "Camera Position: (" << mCamera.x << ", " << mCamera.y << ")\n";


int cameraMarginX = SCREEN_WIDTH / 2;   // Margem horizontal (pode ajustar conforme necessário)
int cameraMarginY = SCREEN_HEIGHT / 2;
 // Obter posição do jogador
int playerCenterX = static_cast<int>(mPlayer.getPosX()) + mPlayer.getWidth() / 2;
int playerCenterY = static_cast<int>(mPlayer.getPosY()) + mPlayer.getHeight() / 2;

// Verificar se o jogador ultrapassou a margem da câmera (horizontal)
if (playerCenterX < mCamera.x + cameraMarginX)
{
    mCamera.x = playerCenterX - cameraMarginX;
}
else if (playerCenterX > mCamera.x + SCREEN_WIDTH - cameraMarginX)
{
    mCamera.x = playerCenterX - (SCREEN_WIDTH - cameraMarginX);
}

// Verificar se o jogador ultrapassou a margem da câmera (vertical)
if (playerCenterY < mCamera.y + cameraMarginY)
{
    mCamera.y = playerCenterY - cameraMarginY;
}
else if (playerCenterY > mCamera.y + SCREEN_HEIGHT - cameraMarginY)
{
    mCamera.y = playerCenterY - (SCREEN_HEIGHT - cameraMarginY);
}



    // Atualiza as caixas
    for (auto &crate : mCrates)
    {
        crate.update(mSolidPlatforms, mWalls, mPlatforms);
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00); // Branco
    SDL_RenderClear(mRenderer);

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &bgRect);

    // Renderizar plataformas, paredes, etc., somente se estiverem visíveis
    for (auto &platform : mPlatforms) {
        if (platform.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT)) {
            platform.render(mRenderer, mCamera.x, mCamera.y);
            std::cout << "Rendering Platform at (" << platform.getX() << ", " << platform.getY() << ")\n";
        }
    }

    for (auto &solidPlatform : mSolidPlatforms) {
        if (solidPlatform.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT)) {
            solidPlatform.render(mRenderer, mCamera.x, mCamera.y);
            std::cout << "Rendering SolidPlatform at (" << solidPlatform.getX() << ", " << solidPlatform.getY() << ")\n";
        }
    }

    for (auto &wall : mWalls) {
        if (wall.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT)) {
            wall.render(mRenderer, mCamera.x, mCamera.y);
            std::cout << "Rendering Wall at (" << wall.getX() << ", " << wall.getY() << ")\n";
        }
    }

    for (auto &crate : mCrates) {
        if (crate.isVisible(mCamera.x, mCamera.y, SCREEN_WIDTH, SCREEN_HEIGHT)) {
            crate.render(mRenderer, mCamera.x, mCamera.y);
            std::cout << "Rendering Crate at (" << crate.getX() << ", " << crate.getY() << ")\n";
        }
    }

    // Renderizar o jogador
    mPlayer.render(mRenderer, mCamera.x, mCamera.y);

    SDL_RenderPresent(mRenderer);
}

void Game::resetGame() {
   mPlayer.reset();
   mCamera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
   mCrates.clear();
   mCrates.push_back(Crate(300, 600, 50, 50));

    
    Mix_PlayMusic(mMusic, -1);
   std::cout << "Resetting game..." << std::endl;

}

bool Game::isRunning()
{
    return !mQuit;
}
