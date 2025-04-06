#include "../include/bettlerider/Game.h"
#include <tinyxml2.h>

using namespace tinyxml2;

namespace BRTC
{
Game::Game(SDL_Window *window, SDL_Renderer *renderer)
    : mWindow(window), mRenderer(renderer), mQuit(false), mPlayer(Vector(0, 0), renderer), mCamera(SCREEN_WIDTH, SCREEN_HEIGHT), mPlayerActivated(true), mActivationTime(0)
{
    std::cout << "Game constructor called" << std::endl;

    mPlatformsTexturePath = "../assets/fulltile.png";

    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
    

    if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
    {
        std::cerr << "Failed to set fullscreen mode: " << SDL_GetError() << std::endl;
    }

    SDL_Surface *loadedBackground =
        IMG_Load("../assets/Background.png");
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

  /*  mMusic = Mix_LoadMUS("../assets/8-bit-game-158815.mp3");
    if (mMusic == nullptr)
    {
        std::cerr << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }*/

   /* mJumpSound = Mix_LoadWAV(
        "../assets/mixkit-player-jumping-in-a-video-game-2043.wav");
    if (mJumpSound == nullptr)
    {
        std::cerr << "Failed to load jump sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }*/

   // Mix_PlayMusic(mMusic, -1);

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

    loadGameLevelFromTMX("../map/level1.tmx");
    mPlayerActivated = false;
    mActivationTime = SDL_GetTicks() + 500;
    Vector playerPos = mPlayer.getPosition();
    mCamera.setPosition(Vector(playerPos.x - (SCREEN_WIDTH/(2*PLAYER_ZOOM_FACTOR)), playerPos.y - (SCREEN_HEIGHT/(2*PLAYER_ZOOM_FACTOR))));
}
static std::unordered_map<std::string, json_object*> levelCache;
Game::~Game()
{
   /* for (auto& entry: levelCache) {
        json_object_put(entry.second);
    }
    levelCache.clear();*/
    Mix_FreeChunk(mJumpSound);
    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
}


void Game::loadGameLevelFromTMX(const std::string &filePath){
    std::cout << "Loading TMX file: " << filePath << std::endl;
    mPlatforms.clear();
    mWalls.clear();
    mSolidPlatforms.clear();
    mCrates.clear();
    mDoors.clear();
    mDecorations.clear();

    XMLDocument doc;
    if (doc.LoadFile(filePath.c_str())) {
        std::cerr << "Failed to load TMX file: " << filePath << std::endl;
        return;
    }

    XMLElement* map = doc.FirstChildElement("map");
    int tileWidth = map->IntAttribute("tilewidth");
    int tileHeight = map->IntAttribute("tileheight");
    const int tileSize = tileWidth;

    mapWidth = map->IntAttribute("width") * tileWidth;
    mapHeight = map->IntAttribute("height") * tileHeight;

    std::unordered_map<int, int> tileTypeMap = {
        {5, 2}, // Plataforma Sólida
        {18, 1}, // Plataforma vazada
        {64, 3}, // Parede
        {65, 4} // Caixote
    };

    XMLElement* layer = map->FirstChildElement("layer");
    while (layer) {
        const char* layerName = layer->Attribute("name");
        XMLElement* data = layer->FirstChildElement("data");

        if (data && strcmp(layerName, "blocks") == 0 || strcmp(layerName, "decorations") == 0) {
            std::string csvData = data->GetText();
            std::istringstream ss(csvData);
            std::string token;

            int index = 0;
            int layerWidth = layer->IntAttribute("width");

            while (std::getline(ss, token, ',')) {
             //   token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
              //  std::cout << "Token: \"" << token << "\"" << std::endl;
                int tileId = std::stoi(token);
                if (tileId != 0) {
                    int x = (index % layerWidth) * tileSize;
                    int y = (index / layerWidth) * tileSize;
                    auto it = tileTypeMap.find(tileId);
                    if (it != tileTypeMap.end()) {
                       
                        if (strcmp(layerName, "blocks") == 0) {
                              switch(it->second) {
                            case 1 : // Plataforma vazada
                                mPlatforms.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                                break;
                            case 2 : // Plataforma sólida
                                mSolidPlatforms.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                                break;
                            case 3 : // Parede
                                mWalls.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                                break;
                            case 4 : // Caixote
                                mCrates.emplace_back(Vector(x, y),  mRenderer);
                                break;
                        }
                        }
                    }
                    if (strcmp(layerName, "decorations") == 0) {
                        mDecorations.emplace_back(Vector(x, y), Vector(tileSize, tileSize), mRenderer, mPlatformsTexturePath);
                        std::cout << "Decoration criada em: " << x << "," << y 
                             << " com tileId: " << tileId << std::endl;
                    }
                   
                }
                index++;
            }
        }
        layer = layer->NextSiblingElement("layer");
    }

    XMLElement* objectGroup = map->FirstChildElement("objectgroup");
    while (objectGroup) {
        XMLElement* obj = objectGroup->FirstChildElement("object");
        while (obj) {
            const char* type = obj->Attribute("type");
            float x = obj->FloatAttribute("x");
            float y = obj->FloatAttribute("y");
    
            if (type && strcmp(type, "player_spawn") == 0) {
                mPlayer.setPosition(Vector(x, y));
            }
            else if (type && strcmp(type, "door") == 0) {
                std::string target;
                float spawnX = 1, spawnY = 1;
    
                XMLElement* properties = obj->FirstChildElement("properties");
                if (properties) {
                    XMLElement* prop = properties->FirstChildElement("property");
                    while (prop) {
                        const char* name = prop->Attribute("name");
                        if (strcmp(name, "target") == 0) {
                            target = prop->Attribute("value");
                        }
                        else if (strcmp(name, "spawn_x") == 0) {
                            spawnX = prop->FloatAttribute("value") * tileSize;
                        }
                        else if (strcmp(name, "spawn_y") == 0) {
                            spawnY = prop->FloatAttribute("value") * tileSize;
                        }
                        prop = prop->NextSiblingElement("property");
                    }
                    mDoors.emplace_back(Vector(x, y), Vector(tileSize, tileSize), target, mRenderer, mPlatformsTexturePath, Vector(spawnX, spawnY));                
                }
            }
            // Avança para o próximo objeto dentro do grupo
            obj = obj->NextSiblingElement("object");
        }
        // Avança para o próximo grupo de objetos
        objectGroup = objectGroup->NextSiblingElement("objectgroup");
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
   // SDL_Log("Game::update() - DeltaTime: %f", deltaTime);
    if(isTransitioning) {
        Vector currentVelocity = mPlayer.getVelocity();
        mPlayer.setVelocity(Vector(0, 0));
        if (SDL_GetTicks() - transitionStartTime > TRANSITION_DELAY) {
            loadGameLevelFromTMX(targetLevel);
            mPlayer.setPosition(targetSpawn);
            mPlayer.setVelocity(currentVelocity);
            mPlayerActivated = false;
            mActivationTime = SDL_GetTicks() + 500;
            isTransitioning = false;
        }
        return;
    }
   
    std::string levelToLoad = "";
    Vector spawnPosition;
    //mPlayer.update();
    
    if (SDL_GetTicks() > mActivationTime) {
        mPlayerActivated = true;
    }
    if (mPlayerActivated) {
        mPlayer.update(deltaTime);
     //   std::cout << "Player ativado após " << (SDL_GetTicks() - mActivationTime) << "ms" << std::endl;
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
  //  SDL_Log("Game::render() chamado");
    
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

  //  bool WeHaveDecorations, WeHavePlatforms, WeHaveSolidPlatforms, WeHaveWalls, WeHaveCrates, WeHaveDoors;

    for (auto &decoration : mDecorations)
    {
        if (decoration.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            decoration.render(mRenderer, mCamera.getPosition());
          //  WeHaveDecorations = true;
       //     std::cout << "We have decorations" << std::endl;
        }
    }

    for (auto &platform : mPlatforms)
    {
        if (platform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            platform.render(mRenderer, mCamera.getPosition());
          //  WeHavePlatforms = true;
          //  std::cout << "We have platforms" << std::endl;
        }
    }

    for (auto &solidPlatform : mSolidPlatforms)
    {
        if (solidPlatform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            solidPlatform.render(mRenderer, mCamera.getPosition());
           // WeHaveSolidPlatforms = true;
       //    std::cout << "We have solid platforms" << std::endl;
        }
    }

    for (auto &wall : mWalls)
    {
        if (wall.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            wall.render(mRenderer, mCamera.getPosition());
           // WeHaveWalls = true;
         //   std::cout << "We have walls" << std::endl;
        }
    }

 

    for (auto &crate : mCrates)
    {
        if (crate.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            crate.render(mRenderer, mCamera.getPosition());
          //  WeHaveCrates = true;
       //     std::cout << "We have crates" << std::endl;
        }
    }

    for (auto &door : mDoors)
    {
        if (door.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            door.render(mRenderer, mCamera.getPosition());
         //   WeHaveDoors = true;
        //    std::cout << "We have doors" << std::endl;
        }
    }

   
        //std::cout << "We have all objects" << std::endl;
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