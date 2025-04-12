#include "../include/bettlerider/Game.h"
#include <tinyxml2.h>

using namespace tinyxml2;

namespace BRTC
{
    Game::Game
    (
        SDL_Window *window, 
        SDL_Renderer *renderer
    )
    : 
    mWindow(window), 
    mRenderer(renderer), 
    mQuit(false), 
    mPlayer(Vector(0, 0), renderer), 
    mCamera(SCREEN_WIDTH, SCREEN_HEIGHT), 
    mPlayerActivated(true), 
    mActivationTime(0)
    {

        std::cout << "Game constructor called" << std::endl;
        mPlatformsTexturePath = "../assets/fulltile.png";
        mPlatformsTexture = IMG_LoadTexture(renderer, mPlatformsTexturePath.c_str());
        if (!mPlatformsTexture)
        {
            std::cerr << "Failed to load platforms texture: " << IMG_GetError() << std::endl;
        }
        SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
    
        const char* layerFiles[5] = 
        {
            "../assets/parallax/1.png",
            "../assets/parallax/2.png",
            "../assets/parallax/3.png",
            "../assets/parallax/4.png",
            "../assets/parallax/5.png"
        };

        mParallaxFactors[0] = 0.1f;
        mParallaxFactors[1] = 0.3f;
        mParallaxFactors[2] = 0.5f;
        mParallaxFactors[3] = 0.7f;
        mParallaxFactors[4] = 0.9f;

        for (int i = 0; i < 5; i++) 
        {
            SDL_Surface *loadedSurface = IMG_Load(layerFiles[i]);
            if (!loadedSurface) 
            {
            std::cerr << "Erro ao carregar  " << layerFiles[i] << ": " << IMG_GetError() << std::endl;
            continue;
            }
            mParallaxLayers[i] = SDL_CreateTextureFromSurface(renderer, loadedSurface);
            SDL_FreeSurface(loadedSurface);
        }
  
        if (TTF_Init() == -1)
        {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
        // Deixei comentada a musica e o som de pulo, elas funcionam, mas não tem necessidade de ficarem ligadas sempre

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
        /*        mFont = TTF_OpenFont("../../platfom2d/assets/All Star Resort.ttf", 100);
        if (mFont == nullptr)
        {
            std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        }

        mSmallFont = TTF_OpenFont("../../platfom2d/assets/Type Machine.ttf", 24);
        if (mSmallFont == nullptr)
        {
            std::cerr << "Failed to load small font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        }*/
        loadGameLevelFromTMX("../map/level1.tmx");
        mPlayerActivated = false;
        mActivationTime = SDL_GetTicks() + 500;
        Vector playerPos = mPlayer.getPosition();
        mCamera.setPosition
        (
            Vector
            (
                playerPos.x - (SCREEN_WIDTH/(2*PLAYER_ZOOM_FACTOR)), 
                playerPos.y - (SCREEN_HEIGHT/(2*PLAYER_ZOOM_FACTOR))
            )
        );
    }
    Game::~Game()
    {
        for (int i = 0; i < 5; i++) 
        {
            if (mParallaxLayers[i]) {SDL_DestroyTexture(mParallaxLayers[i]);}
        }
        Mix_FreeChunk(mJumpSound);
        TTF_CloseFont(mFont);
        TTF_CloseFont(mSmallFont);
        TTF_Quit();
    }


    void Game::loadGameLevelFromTMX(const std::string &filePath)
    {
        std::cout << "Loading TMX file: " << filePath << std::endl;
        clearLevelData();
        XMLDocument doc;
        if (doc.LoadFile(filePath.c_str())) 
        {
            std::cerr << "Failed to load TMX file: " << filePath << std::endl;
            return;
        }
        XMLElement* map = doc.FirstChildElement("map");
        int tileSize = map->IntAttribute("tilewidth");
        mapWidth = map->IntAttribute("width") * tileSize;
        mapHeight = map->IntAttribute("height") * tileSize;
        processMapLayers(map, tileSize);
        processObjectGroups(map, tileSize);
    }

    void Game::clearLevelData()
    {
        mPlatforms.clear();
        mWalls.clear();
        mSolidPlatforms.clear();
        mCrates.clear();
        mDoors.clear();
        mDecorations.clear();
    }

    void Game::processMapLayers(XMLElement* map, int tileSize) 
    {
        std::unordered_map<int, int> tileTypeMap = 
        {
        {5, 2},{9, 2},{16, 2}, {31, 2},  // Plataforma Sólida
        {18, 1}, // Plataforma vazada
        {64, 3},{15, 3},{41, 3}, {87, 3}, // Parede
        {65, 4} // Caixote
        };
        XMLElement* layer = map->FirstChildElement("layer");
        while (layer) {
            processLayers(layer, tileSize, tileTypeMap);
            layer = layer->NextSiblingElement("layer");
        }
    }

    void Game::processLayers
    (
        XMLElement* layer, 
        int tileSize, 
        const std::unordered_map<int, int>& tileTypeMap
    ) 
    {
        const char* layerName = layer->Attribute("name");
        XMLElement* data = layer->FirstChildElement("data"); 
        if 
        (
            !data || 
            !(strcmp(layerName, "blocks") == 0 || 
            strcmp(layerName, "decorations") == 0)
        ) 
        {
            return;
        }  
        std::string csvData = data->GetText();
        std::istringstream ss(csvData);
        std::string token;
        int index = 0;
        int layerWidth = layer->IntAttribute("width");
        while (std::getline(ss, token, ',')) 
        {
            int tileId = std::stoi(token);
            if (tileId == 0) 
            {
                index++;
                continue;
            }
            int x = (index % layerWidth) * tileSize;
            int y = (index / layerWidth) * tileSize;
            if (strcmp(layerName, "blocks") == 0) 
            {
                processBlockTile(tileId, x, y, tileSize, tileTypeMap);
            } 
            else if (strcmp(layerName, "decorations") == 0) 
            {
                mDecorations.emplace_back
                (
                    Vector(x, y), 
                    Vector(tileSize, tileSize), 
                    mPlatformsTexture, 
                    tileId
                );
            }
            index++;
        }
    } 

    void Game::processBlockTile
    (
        int tileId, 
        int x, 
        int y, 
        int tileSize, 
        const std::unordered_map<int, int>& tileTypeMap
    ) 
    {
        auto it = tileTypeMap.find(tileId);
        if (it == tileTypeMap.end()) return;
        switch(it->second) 
        {
            case 1: // Plataforma vazada
            mPlatforms.emplace_back
            (
                Vector(x, y), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 2: // Plataforma sólida
            mSolidPlatforms.emplace_back
            (
                Vector(x, y), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 3: // Parede
            mWalls.emplace_back
            (
                Vector(x, y), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 4: // Caixote
            mCrates.emplace_back
            (
                Vector(x, y),  
                mRenderer
            );
            break;
            }
    }

    void Game::processObjectGroups(XMLElement* map, int tileSize) 
    {
        XMLElement* objectGroup = map->FirstChildElement("objectgroup");
        while (objectGroup) 
        {
            processObjectGroup(objectGroup, tileSize);
            objectGroup = objectGroup->NextSiblingElement("objectgroup");
        }
    }

    void Game::processObjectGroup(XMLElement* objectGroup, int tileSize) 
    {
        XMLElement* obj = objectGroup->FirstChildElement("object");
        while (obj) 
        {
            processObject(obj, tileSize);
            obj = obj->NextSiblingElement("object");
        }
    }

    void Game::processObject(XMLElement* obj, int tileSize) 
    {
        const char* type = obj->Attribute("type");
        float x = obj->FloatAttribute("x");
        float y = obj->FloatAttribute("y");
        if (!type) return;
        if (strcmp(type, "player_spawn") == 0) { mPlayer.setPosition(Vector(x, y)); } 
        else if(strcmp(type, "door") == 0) { processDoorObject(obj, x, y, tileSize); }
    }

    void Game::processDoorObject
    (
        XMLElement* obj, 
        float x, 
        float y, 
        int tileSize
    ) 
    {
        std::string target;
        float spawnX = 1, spawnY = 1;
        XMLElement* properties = obj->FirstChildElement("properties");
        if (!properties) return;
        XMLElement* prop = properties->FirstChildElement("property");
        while (prop) 
        {
            const char* name = prop->Attribute("name");
            if (!name) continue;
            if (strcmp(name, "target") == 0) { target = prop->Attribute("value"); } 
            else if (strcmp(name, "spawn_x") == 0) { spawnX = prop->FloatAttribute("value") * tileSize; } 
            else if (strcmp(name, "spawn_y") == 0) { spawnY = prop->FloatAttribute("value") * tileSize; }
            prop = prop->NextSiblingElement("property");
        }
        mDoors.emplace_back
        (
            Vector(x, y), 
            Vector(tileSize, tileSize), 
            target, 
            mRenderer, 
            mPlatformsTexturePath, 
            Vector(spawnX, spawnY)
        );
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
     //       Mix_PlayChannel(-1, mJumpSound, 0);

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
            std::cout << "Spawning at: " << targetSpawn.x << ", " << targetSpawn.y << std::endl;
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
    
    if (SDL_GetTicks() > mActivationTime) {
        mPlayerActivated = true;
    }
    if (mPlayerActivated) {
        mPlayer.update(deltaTime);
    }
    PhysicsEngine::HandleCollisions(
        mPlayer, mWalls, mPlatforms, mSolidPlatforms);
    if (PhysicsEngine::HandlePlayerCollisions(
        mPlayer, spawnPosition, mDoors, mCrates, levelToLoad))
    { 
       if(!isTransitioning) {
            isTransitioning = true;
            transitionStartTime = SDL_GetTicks();
            targetLevel = levelToLoad;
            targetSpawn = spawnPosition;
       }
    }
  
    Vector playerPosition = mPlayer.getPosition();


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

    Vector cameraPos = mCamera.getPosition();
    
    for (int i = 0; i < 5; i++) {
        if (mParallaxLayers[i]) {
            int texWidth, texHeight;
            SDL_QueryTexture(mParallaxLayers[i], nullptr, nullptr, &texWidth, &texHeight);

            float factor = mParallaxFactors[i];
            Vector parallaxOffset;
            parallaxOffset.x = cameraPos.x * factor;
            parallaxOffset.y = cameraPos.y * factor * 0.5f;

            // Calcula a posição base para repetição
            Vector base;
            base.x = -parallaxOffset.x;
            base.y = -parallaxOffset.y;

            // Calcula quantas vezes a textura precisa se repetir
            Vector repeat;
            repeat.x = static_cast<int>(effectiveScreenWidth / texWidth) + 2;
            repeat.y = static_cast<int>(effectiveScreenHeight / texHeight) + 2;

            // Ajusta o ponto inicial para preencher toda a tela
            Vector start;
            start.x = static_cast<int>(base.x) % texWidth;
            start.y = static_cast<int>(base.y) % texHeight;
            if (start.x > 0) start.x -= texWidth;
            if (start.y > 0) start.y -= texHeight;

            // Renderiza as repetições da textura
            for (int x = 0; x < repeat.x; x++) {
                for (int y = 0; y < repeat.y; y++) {
                    SDL_Rect layerRect = {
                        static_cast<int>(start.x + x * texWidth),
                        static_cast<int>(start.y + y * texHeight),
                        texWidth,
                        texHeight
                    };
                    SDL_RenderCopy(mRenderer, mParallaxLayers[i], nullptr, &layerRect);
                }
            }
        }
    }

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
        }
    }

    for (auto &solidPlatform : mSolidPlatforms)
    {
        if (solidPlatform.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            solidPlatform.render(mRenderer, mCamera.getPosition());
        }
    }

    for (auto &wall : mWalls)
    {
        if (wall.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            wall.render(mRenderer, mCamera.getPosition());
        }
    }

 

    for (auto &crate : mCrates)
    {
        if (crate.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            crate.render(mRenderer, mCamera.getPosition());
        }
    }

    for (auto &door : mDoors)
    {
        if (door.isVisible(mCamera.getPosition(), Vector(effectiveScreenWidth, effectiveScreenHeight)))
        {
            door.render(mRenderer, mCamera.getPosition());
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