#include "../include/bettlerider/Game.h"

namespace BRTC
{
Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false), mPlayer(Vector(0, 0), renderer), mCamera(SCREEN_WIDTH, SCREEN_HEIGHT), mPlayerActivated(true), mActivationTime(0)
{
    std::cout << "Game constructor called" << std::endl;

    mPlatformsTexturePath = "../assets/fulltile.png";

    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
    SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);

    if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
    {
        std::cerr << "Failed to set fullscreen mode: " << SDL_GetError() << std::endl;
    }

    SDL_Surface *loadedBackground =
        IMG_Load("/home/netorapg/projects/platfom2d/assets/166722.p");
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
    Vector playerPos = mPlayer.getPosition();
    mCamera.setPosition(Vector(playerPos.x - SCREEN_WIDTH / 2, playerPos.y - SCREEN_HEIGHT / 2));
}
static std::unordered_map<std::string, json_object*> levelCache;
Game::~Game()
{
    for (auto& entry: levelCache) {
        json_object_put(entry.second);
    }
    levelCache.clear();
    Mix_FreeChunk(mJumpSound);
    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
}



void Game::loadLevelFromJSON(const std::string &filePath) {
    mPlatforms.clear();
    mSolidPlatforms.clear();
    mWalls.clear();
    mCrates.clear();
    mDoors.clear();

    if(levelCache.find(filePath) == levelCache.end()) {
        json_object *root = json_object_from_file(filePath.c_str());
        if (!root) {
            std::cerr << "Failed to load JSON file: " << filePath << std::endl;
            return;
        }
        levelCache[filePath] = root;
    }
    json_object *root = levelCache[filePath];

    // Obter informações do mapa
    int tileWidth = json_object_get_int(json_object_object_get(root, "tilewidth"));
    int tileHeight = json_object_get_int(json_object_object_get(root, "tileheight"));
    const int tileSize = tileWidth; // Assumindo tiles quadrados

    // Mapeamento de GID para tipos de tile (ajuste conforme seu tileset)
    std::unordered_map<int, int> tileTypeMap = {
        {5, 2}, // Plataforma sólida
        {18, 1}, // Plataforma
        {64, 3}, // Parede
        {65, 4}  // Caixote
    };

    // Processar camadas
    json_object *layers;
    json_object_object_get_ex(root, "layers", &layers);
    int numLayers = json_object_array_length(layers);

    for (int l = 0; l < numLayers; ++l) {
        json_object *layer = json_object_array_get_idx(layers, l);
        const char *layerName = json_object_get_string(json_object_object_get(layer, "name"));

        if (strcmp(layerName, "Decorations") == 0) {
            json_object *data;
            json_object_object_get_ex(layer, "data", &data);

            for (int i = 0; i < json_object_array_length(data); ++i) {
                int tileId = json_object_get_int(json_object_array_get_idx(data, i));
                if (tileId == 0) continue;

                int x = (i % tileWidth) * tileSize;
                int y = (i / tileWidth) * tileSize;

                mDecorations.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
            }
            continue;
        }
    }

    for (int l = 0; l < numLayers; ++l) {
        json_object *layer = json_object_array_get_idx(layers, l);
        const char *layerType;
        json_object *typeObj;
        json_object_object_get_ex(layer, "type", &typeObj);
        layerType = json_object_get_string(typeObj);
        

        // Camada de tiles
        if (strcmp(layerType, "tilelayer") == 0) {
            json_object *data;
            json_object_object_get_ex(layer, "data", &data);
            int layerWidth = json_object_get_int(json_object_object_get(layer, "width"));
            int layerHeight = json_object_get_int(json_object_object_get(layer, "height"));

            for (int i = 0; i < json_object_array_length(data); ++i) {
                int tileId = json_object_get_int(json_object_array_get_idx(data, i));
                if (tileId == 0) continue; // Tile vazio

                int tileType = tileTypeMap[tileId];
                int x = (i % layerWidth) * tileSize;
                int y = (i / layerWidth) * tileSize;

                switch (tileType) {
                    case 1:
                        mPlatforms.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                        break;
                    case 2:
                        mSolidPlatforms.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                        break;
                    case 3:
                        mWalls.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                        break;
                    case 4:
                        mCrates.emplace_back(Vector(x, y), mRenderer);
                        break;
                }
            }
        }

        // Camada de objetos
        if (strcmp(layerType, "objectgroup") == 0) {
            json_object *objects;
            json_object_object_get_ex(layer, "objects", &objects);

            for (int i = 0; i < json_object_array_length(objects); ++i) {
                json_object *obj = json_object_array_get_idx(objects, i);
                const char *objType = "";
                json_object *typeObj;
                if (json_object_object_get_ex(obj, "type", &typeObj)) {
                    objType = json_object_get_string(typeObj);
                }

                // Player Spawn
                if (strcmp(objType, "player_spawn") == 0) {
                    float x = json_object_get_double(json_object_object_get(obj, "x"));
                    float y = json_object_get_double(json_object_object_get(obj, "y"));
                    mPlayer.setPosition(Vector(x, y));
                }

                // Portas
                if (strcmp(objType, "door") == 0) {
                    float x = json_object_get_double(json_object_object_get(obj, "x"));
                    float y = json_object_get_double(json_object_object_get(obj, "y"));
                    std::string target;
                    float spawnX = -1, spawnY = -1;

                    json_object *props;
                    if (json_object_object_get_ex(obj, "properties", &props)) {
                        for (int p = 0; p < json_object_array_length(props); ++p) {
                            json_object *prop = json_object_array_get_idx(props, p);
                            const char *name = json_object_get_string(json_object_object_get(prop, "name"));
                            if (strcmp(name, "target") == 0) {
                                target = json_object_get_string(json_object_object_get(prop, "value"));
                            } else if (strcmp(name, "spawn_x") == 0) {
                                spawnX = json_object_get_double(json_object_object_get(prop, "value")) * tileSize;
                            } else if (strcmp(name, "spawn_y") == 0) {
                                spawnY = json_object_get_double(json_object_object_get(prop, "value")) * tileSize;
                            }
                        }
                    }
                    mDoors.emplace_back(Vector(x, y), Vector(20, 62), target, mRenderer, mPlatformsTexturePath, Vector(spawnX, spawnY));
                }

                mapWidth = json_object_get_int(json_object_object_get(root, "width")) * tileSize;
                mapHeight = json_object_get_int(json_object_object_get(root, "height")) * tileSize;
            }
        }
    }
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
    SDL_Log("Game::update() - DeltaTime: %f", deltaTime);
    if(isTransitioning) {
        
        if (SDL_GetTicks() - transitionStartTime > TRANSITION_DELAY) {
            loadLevelFromJSON(targetLevel);
            mPlayer.setPosition(targetSpawn);
            mPlayerActivated = false;
            mActivationTime = SDL_GetTicks();
            isTransitioning = false;
        }
        return;
    }
   
    std::string levelToLoad = "";
    Vector spawnPosition;
    //mPlayer.update();
    if (!mPlayerActivated && SDL_GetTicks() > mActivationTime) {
        mPlayerActivated = true;
    }
    if (mPlayerActivated) {
        mPlayer.update(deltaTime);
    }
    PhysicsEngine::HandleCollisions(
        mPlayer, mWalls, mPlatforms, mSolidPlatforms);
    if (PhysicsEngine::HandlePlayerCollisions(
        mPlayer, mCrates, mDoors, levelToLoad, spawnPosition))
    { 
        /*Vector newSpawn = (spawnPosition.x >= 0 && spawnPosition.y >= 0) ? spawnPosition : Vector(-1, -1);

        loadLevelFromJSON(levelToLoad);
        if (newSpawn.x >= 0 && newSpawn.y >= 0) {
            mPlayer.setPosition(newSpawn);
        }*/
       if(!isTransitioning) {
            isTransitioning = true;
            transitionStartTime = SDL_GetTicks();
            targetLevel = levelToLoad;
            targetSpawn = spawnPosition;
       }
    }

    Vector playerPosition = mPlayer.getPosition();

    /*std::cout << "Player Position: (" << playerPosition.x << ", "
              << playerPosition.y << ")\n";
    std::cout << "Camera Position: (" << mCamera.getPosition().x << ", " << mCamera.getPosition().y << ")\n";
    std::cout << "mOnGround: " << mPlayer.isOnGround()
              << ", mFalling: " << mPlayer.isFalling() << std::endl;*/

    float cameraMarginX = effectiveScreenWidth * 0.50f;
    float cameraMarginY = effectiveScreenHeight * 0.50f;

    // Mantenha o jogador centralizado na área visível reduzida
    float playerCenterX = playerPosition.x + mPlayer.getWidth() / 2;
    float playerCenterY = playerPosition.y + mPlayer.getHeight() / 2;

    // Atualize a posição da câmera
    mCamera.setPosition(Vector(playerCenterX - effectiveScreenWidth / 2, playerCenterY - effectiveScreenHeight / 2));
    Vector cameraPosition = mCamera.getPosition();
    cameraPosition.x = std::max(0.0f, std::min(cameraPosition.x, static_cast<float>(mapWidth) - effectiveScreenWidth));
    cameraPosition.y = std::max(0.0f, std::min(cameraPosition.y, static_cast<float>(mapHeight) - effectiveScreenHeight));
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
        crate.update(deltaTime); // Passe um valor de tempo delta apropriado
        PhysicsEngine::HandleCollisions(
            crate, mWalls, mPlatforms, mSolidPlatforms);
    }
}

void Game::render()
{
    SDL_Log("Game::render() chamado");
    if (isTransitioning) {
        Uint32 elapsed = SDL_GetTicks() - transitionStartTime;
        if (elapsed <= HALF_TRANSITION) {
            float progress = static_cast<float>(elapsed) / HALF_TRANSITION;
            alpha = static_cast<int>(255 * progress);
        } else if (elapsed < TRANSITION_DELAY) {
            float progress = static_cast<float>(elapsed - HALF_TRANSITION) / HALF_TRANSITION;
            alpha = 255 - static_cast<int>(255 * progress);
        } else {
            alpha = 0;
        }   
    
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_Rect fadeRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(mRenderer, &fadeRect);
        SDL_RenderPresent(mRenderer);
        return;
    }

    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    SDL_RenderClear(mRenderer);

    SDL_Rect bgRect = {0, 0, static_cast<int>(effectiveScreenWidth), static_cast<int>(effectiveScreenHeight)};
    SDL_RenderCopy(mRenderer, mBackgroundTexture, nullptr, &bgRect);

    bool WeHaveDecorations, WeHavePlatforms, WeHaveSolidPlatforms, WeHaveWalls, WeHaveCrates, WeHaveDoors;

    for (auto &decoration : mDecorations)
    {
        if (decoration.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            decoration.render(mRenderer, mCamera.getPosition());
            WeHaveDecorations = true;
        }
    }

    for (auto &platform : mPlatforms)
    {
        if (platform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            platform.render(mRenderer, mCamera.getPosition());
            WeHavePlatforms = true;
        }
    }

    for (auto &solidPlatform : mSolidPlatforms)
    {
        if (solidPlatform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            solidPlatform.render(mRenderer, mCamera.getPosition());
            WeHaveSolidPlatforms = true;
        }
    }

    for (auto &wall : mWalls)
    {
        if (wall.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            wall.render(mRenderer, mCamera.getPosition());
            WeHaveWalls = true;
        }
    }

 

    for (auto &crate : mCrates)
    {
        if (crate.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            crate.render(mRenderer, mCamera.getPosition());
            WeHaveCrates = true;
        }
    }

    for (auto &door : mDoors)
    {
        if (door.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            door.render(mRenderer, mCamera.getPosition());
            WeHaveDoors = true;
        }
    }

    if (WeHaveDecorations == true && WeHavePlatforms == true && WeHaveSolidPlatforms == true && WeHaveWalls == true && WeHaveCrates == true && WeHaveDoors == true)
    {
        std::cout << "We have all objects" << std::endl;
        mPlayer.render(mRenderer, mCamera.getPosition());
    }
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