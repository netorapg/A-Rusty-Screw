#include "../include/bettlerider/Game.h"

namespace BRTC
{
Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false), mPlayer(Vector(0, 0), renderer), mCamera(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    std::cout << "Game constructor called" << std::endl;

    mPlatformsTexturePath = "../assets/scenario.png";

    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);

    if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
    {
        std::cerr << "Failed to set fullscreen mode: " << SDL_GetError() << std::endl;
    }

    SDL_Surface *loadedBackground =
        IMG_Load("/home/netorapg/projects/platfom2d/assets/166722.png");
    if (loadedBackground == nullptr)
    {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
    }
    mBackgroundTexture =
        SDL_CreateTextureFromSurface(renderer, loadedBackground);
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

    mJumpSound = Mix_LoadWAV(
        "../../platfom2d/assets/mixkit-player-jumping-in-a-video-game-2043.wa");
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
    Mix_FreeChunk(mJumpSound);
    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
}

void Game::loadLevelFromJSON(const std::string &filePath)
{
    // Limpar listas ou variáveis antes de carregar o novo conteúdo
    mPlatforms.clear();
    mSolidPlatforms.clear();
    mWalls.clear();
    mCrates.clear();
    mDoors.clear();

    json_object *root = json_object_from_file(filePath.c_str());

    if (!root)
    {
        std::cerr << "Failed to load JSON file: " << filePath << std::endl;
        return;
    }

    json_object *tiles;
    if (!json_object_object_get_ex(root, "tiles", &tiles))
    {
        std::cerr << "Invalid JSON: missing 'tiles'" << std::endl;
        json_object_put(root);
        return;
    }

    std::unordered_map<std::string, int> tileMap;
    json_object_object_foreach(tiles, key, val)
    {
        tileMap[key] = json_object_get_int(val);
    }

    json_object *layers;
    if (!json_object_object_get_ex(root, "layers", &layers))
    {
        std::cerr << "Invalid JSON: missing 'layers'" << std::endl;
        json_object_put(root);
        return;
    }

    json_object *first_layer = json_object_array_get_idx(layers, 0);
    if (!first_layer)
    {
        std::cerr << "Invalid JSON: no layers found" << std::endl;
        json_object_put(root);
        return;
    }

    json_object *data;
    if (!json_object_object_get_ex(first_layer, "data", &data))
    {
        std::cerr << "Invalid JSON: missing 'data'" << std::endl;
        json_object_put(root);
        return;
    }

    const int tileSize = 30;
    const int rows = json_object_array_length(data);
    for (int i = 0; i < rows; ++i)
    {
        json_object *row = json_object_array_get_idx(data, i);
        const int cols = json_object_array_length(row);

        for (int j = 0; j < cols; ++j)
        {
            json_object *tile = json_object_array_get_idx(row, j);
            const char *tileName = json_object_get_string(tile);

            if (tileMap.find(tileName) == tileMap.end())
            {
                std::cerr << "Unknown tile name: " << tileName << std::endl;
                continue;
            }

            int tileType = tileMap[tileName];
            int x = j * tileSize;
            int y = i * tileSize;

            switch (tileType)
            {
            case 0: // Espaço vazio
                break;
            case 1: // Plataforma
                mPlatforms.emplace_back(Vector(x, y), Vector(30, 20), mRenderer, mPlatformsTexturePath);
                break;
            case 2: // Plataforma sólida
                mSolidPlatforms.emplace_back(Vector(x, y), Vector(30, 40), mRenderer, mPlatformsTexturePath);
                break;
            case 3: // Parede
                mWalls.emplace_back(Vector(x, y), Vector(20, 40), mRenderer, mPlatformsTexturePath);
                break;
            case 4: // Caixote
                mCrates.emplace_back(Vector(x, y), mRenderer);
                break;
            default:
                std::cerr << "Unknown tile type: " << tileType << std::endl;
            }
        }
    }
    json_object *doors;
    if (json_object_object_get_ex(root, "doors", &doors))
    {
        for (int i = 0; i < json_object_array_length(doors); ++i)
        {
            json_object *door = json_object_array_get_idx(doors, i);
            float x = json_object_get_double(json_object_object_get(door, "x")) * tileSize;
            float y = json_object_get_double(json_object_object_get(door, "y")) * tileSize;
            float spawnX = -1.0f;
            float spawnY = -1.0f;
            json_object* spawnXObj;
            if (json_object_object_get_ex(door, "spawn_x", &spawnXObj)) {
                spawnX = json_object_get_double(spawnXObj) * tileSize;
            }
            json_object* spawnYObj;
            if (json_object_object_get_ex(door, "spawn_y", &spawnYObj)) {
                spawnY = json_object_get_double(spawnYObj) * tileSize;
            }

            std::string target = json_object_get_string(json_object_object_get(door, "target"));
            mDoors.emplace_back(Vector(x, y), Vector(20, 62), target, mRenderer, mPlatformsTexturePath, Vector(spawnX, spawnY));
        }
    }

    

    json_object *player_spawn;
    if (json_object_object_get_ex(root, "player_spawn", &player_spawn)) {
        float spawnX = json_object_get_int(json_object_object_get(player_spawn, "x")) * tileSize;
        float spawnY = json_object_get_int(json_object_object_get(player_spawn, "y")) * tileSize;
        
        if (mPlayer.getPosition() == Vector(0, 0)) {
            mPlayer.setPosition(Vector(spawnX, spawnY));
        }
    } else {
        if (mPlayer.getPosition() == Vector(0, 0)) {
            mPlayer.setPosition(Vector(0, 0));
        }
    }

    json_object_put(root);
}

void Game::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            mQuit = true;
        mPlayer.handleEvent(e);

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
            resetGame();

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
            Mix_PlayChannel(-1, mJumpSound, 0);

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11)
        {
            Uint32 flags = SDL_GetWindowFlags(mWindow);
            if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                SDL_SetWindowFullscreen(mWindow, 0);
            else
                SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
    }
}

void Game::update()
{
    mPlayer.update(0.8f);
    std::string levelToLoad = "";
    Vector spawnPosition;
    PhysicsEngine::HandleCollisions(
        mPlayer, mWalls, mPlatforms, mSolidPlatforms);
    if (PhysicsEngine::HandlePlayerCollisions(
        mPlayer, mCrates, mDoors, levelToLoad, spawnPosition))
    { 
        Vector newSpawn = (spawnPosition.x >= 0 && spawnPosition.y >= 0) ? spawnPosition : Vector(-1, -1);

        loadLevelFromJSON(levelToLoad);
        if (newSpawn.x >= 0 && newSpawn.y >= 0) {
            mPlayer.setPosition(newSpawn);
        }
    }

    Vector playerPosition = mPlayer.getPosition();

    std::cout << "Player Position: (" << playerPosition.x << ", "
              << playerPosition.y << ")\n";
    std::cout << "Camera Position: (" << mCamera.getPosition().x << ", " << mCamera.getPosition().y << ")\n";
    std::cout << "mOnGround: " << mPlayer.isOnGround()
              << ", mFalling: " << mPlayer.isFalling() << std::endl;

    float cameraMarginX = effectiveScreenWidth * 0.50f;
    float cameraMarginY = effectiveScreenHeight * 0.50f;

    // Mantenha o jogador centralizado na área visível reduzida
    float playerCenterX = playerPosition.x + mPlayer.getWidth() / 2;
    float playerCenterY = playerPosition.y + mPlayer.getHeight() / 2;

    // Atualize a posição da câmera
    mCamera.setPosition(Vector(playerCenterX - effectiveScreenWidth / 2, playerCenterY - effectiveScreenHeight / 2));
    Vector cameraPosition = mCamera.getPosition();
    cameraPosition.x = std::max(0.0f, std::min(cameraPosition.x, static_cast<float>(LEVEL_WIDTH) - effectiveScreenWidth));
    cameraPosition.y = std::max(0.0f, std::min(cameraPosition.y, static_cast<float>(LEVEL_HEIGHT) - effectiveScreenHeight));
    mCamera.setPosition(cameraPosition);

    if (playerCenterX < cameraPosition.x + cameraMarginX)
        cameraPosition.x = playerCenterX - cameraMarginX;
    else if (playerCenterX > cameraPosition.x + SCREEN_WIDTH - cameraMarginX)
        cameraPosition.x = playerCenterX - (SCREEN_WIDTH - cameraMarginX);

    if (playerCenterY < cameraPosition.y + cameraMarginY)
        cameraPosition.y = playerCenterY - cameraMarginY;
    else if (playerCenterY > cameraPosition.y + SCREEN_HEIGHT - cameraMarginY)
        cameraPosition.y = playerCenterY - (SCREEN_HEIGHT - cameraMarginY);

    mCamera.setPosition(cameraPosition);

    for (auto &crate : mCrates)
    {
        crate.update(1.0f); // Passe um valor de tempo delta apropriado
        PhysicsEngine::HandleCollisions(
            crate, mWalls, mPlatforms, mSolidPlatforms);
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    SDL_RenderClear(mRenderer);

    SDL_Rect bgRect = {0, 0, static_cast<int>(effectiveScreenWidth), static_cast<int>(effectiveScreenHeight)};
    SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &bgRect);

    for (auto &platform : mPlatforms)
    {
        if (platform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            platform.render(mRenderer, mCamera.getPosition());
            Vector platformPosition = platform.getPosition();
            std::cout << "Rendering Platform at (" << platformPosition.x << ", "
                      << platformPosition.y << ")\n";
        }
    }

    for (auto &solidPlatform : mSolidPlatforms)
    {
        if (solidPlatform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            solidPlatform.render(mRenderer, mCamera.getPosition());
            Vector solidPlatformPosition = solidPlatform.getPosition();
            std::cout << "Rendering SolidPlatform at (" << solidPlatformPosition.x
                      << ", " << solidPlatformPosition.y << ")\n";
        }
    }

    for (auto &wall : mWalls)
    {
        if (wall.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            wall.render(mRenderer, mCamera.getPosition());
            Vector wallPosition = wall.getPosition();
            std::cout << "Rendering Wall at (" << wallPosition.x << ", " << wallPosition.y
                      << ")\n";
        }
    }

    for (auto &crate : mCrates)
    {
        if (crate.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            crate.render(mRenderer, mCamera.getPosition());
            Vector cratePosition = crate.getPosition();
            std::cout << "Rendering Crate at (" << cratePosition.x << ", "
                      << cratePosition.y << ")\n";
        }
    }

    for (auto &door : mDoors)
    {
        if (door.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            door.render(mRenderer, mCamera.getPosition());
            Vector doorPosition = door.getPosition();
            std::cout << "Rendering Door at (" << doorPosition.x << ", " << doorPosition.y
                      << ")\n";
        }
    }

    mPlayer.render(mRenderer, mCamera.getPosition());
    SDL_RenderPresent(mRenderer);
}

void Game::resetGame()
{
    mPlayer.setVelocity(Vector(0, 0));   // Velocidade inicial
    mCamera.setPosition(Vector(0, 0));

    std::cout << "Resetting game..." << std::endl;
}

bool Game::isRunning()
{
    return !mQuit;
}

} // namespace BRTC