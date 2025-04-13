#include "../include/bettlerider/Game.h"
#include <tinyxml2.h>

using namespace tinyxml2;

namespace BRTC
{
/*
-------------------------------------------------------------------------------
    Construtor da classe Game.
    Inicializa a janela, o renderizador, e carrega os recursos necessários.
    Configura o sistema de áudio e carrega a textura de fundo.
-------------------------------------------------------------------------------
*/
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
    mActivationTime(0),
    mPlatformsTexture(nullptr),
    mMusic(nullptr),
    mJumpSound(nullptr),
    mFont(nullptr),
    mSmallFont(nullptr),
    isTransitioning(false),
    increasing(true),
    alpha(0)
    {
        std::cout << "Game constructor called" << std::endl;
        initializeRenderSettings();
        loadTextures();
        initializeAudioSystem();
        loadInitialLevel();
        mPlayerActivated = false;
        mActivationTime = SDL_GetTicks() + 500;
        centerCameraOnPlayer();
    }

    void Game::initializeRenderSettings()
    {
        SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
    }

    void Game::loadTextures()
    {
        mPlatformsTexturePath = "assets/fulltile.png";
        mPlatformsTexture = IMG_LoadTexture(mRenderer, mPlatformsTexturePath.c_str());
        if (!mPlatformsTexture)
        {
            std::cerr << "Failed to load platforms texture: " << IMG_GetError() << std::endl;
        }
    }

    void Game::initializeAudioSystem() 
    {
        if (TTF_Init() == -1) 
        {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
        {
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
        // Audio loading commented out as per original code
        /*
        mMusic = Mix_LoadMUS("../assets/8-bit-game-158815.mp3");
        if (!mMusic) {
            std::cerr << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
        */
    }

    void Game::loadParallaxLayers() 
    {
        const std::array<const char*, 5> layerFiles = 
        {
            "../assets/parallax/1.png",
            "../assets/parallax/2.png",
            "../assets/parallax/3.png",
            "../assets/parallax/4.png",
            "../assets/parallax/5.png"
        };
        const std::array<float, 5> factors = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
        mParallaxFactors = factors;
        for (int i = 0; i < 5; i++) 
        {
            SDL_Surface* loadedSurface = IMG_Load(layerFiles[i]);
            if (!loadedSurface) 
            {
                std::cerr << "Error loading " << layerFiles[i] << ": " << IMG_GetError() << std::endl;
                continue;
            }
            mParallaxLayers[i] = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
            SDL_FreeSurface(loadedSurface);
            if (!mParallaxLayers[i]) 
            {
                std::cerr << "Error creating texture for layer " << i << std::endl;
            }
        }
    }
    
    void Game::loadInitialLevel() 
    {
        loadGameLevelFromTMX("../map/level1.tmx");
    }
    
    void Game::centerCameraOnPlayer() 
    {
        Vector playerPos = mPlayer.getPosition();
        mCamera.setPosition
        (
            Vector
            (
            playerPos.x - (SCREEN_WIDTH / (2 * PLAYER_ZOOM_FACTOR)),
            playerPos.y - (SCREEN_HEIGHT / (2 * PLAYER_ZOOM_FACTOR))
            )
        );
    }
/*
-------------------------------------------------------------------------------
    Destrutor da classe Game.
    Libera os recursos alocados, como texturas, fontes e áudio.
-------------------------------------------------------------------------------
*/
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
/*
-------------------------------------------------------------------------------
    Função para carregar o arquivo TMX e processar os dados do mapa.
    Ela carrega o arquivo, processa as camadas e os objetos, e armazena os dados
    em listas apropriadas.
-------------------------------------------------------------------------------
*/
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
            mTilePosition.x = (index % layerWidth) * tileSize;
            mTilePosition.y = (index / layerWidth) * tileSize;
            if (strcmp(layerName, "blocks") == 0) 
            {
                processBlockTile(tileId, mTilePosition, tileSize, tileTypeMap);
            } 
            else if (strcmp(layerName, "decorations") == 0) 
            {
                mDecorations.emplace_back
                (
                    Vector(mTilePosition), 
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
        Vector& tilePosition,
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
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 2: // Plataforma sólida
            mSolidPlatforms.emplace_back
            (
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 3: // Parede
            mWalls.emplace_back
            (
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
            case 4: // Caixote
            mCrates.emplace_back
            (
                Vector(mTilePosition),  
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
        mAttributeSpawn.x = obj->FloatAttribute("x");
        mAttributeSpawn.y = obj->FloatAttribute("y");
        if (!type) return;
        if (strcmp(type, "player_spawn") == 0) { mPlayer.setPosition(Vector(mAttributeSpawn)); } 
        else if(strcmp(type, "door") == 0) { processDoorObject(obj, mAttributeSpawn, tileSize); }
    }

    void Game::processDoorObject
    (
        XMLElement* obj, 
        Vector& AttributeSpawn,
        int tileSize
    ) 
    {
        std::string target;
        XMLElement* properties = obj->FirstChildElement("properties");
        if (!properties) return;
        XMLElement* prop = properties->FirstChildElement("property");
        while (prop) 
        {
            const char* name = prop->Attribute("name");
            if (!name) continue;
            if (strcmp(name, "target") == 0) { target = prop->Attribute("value"); } 
            else if (strcmp(name, "spawn_x") == 0) { mSpawnPosition.x = prop->FloatAttribute("value") * tileSize; } 
            else if (strcmp(name, "spawn_y") == 0) { mSpawnPosition.y = prop->FloatAttribute("value") * tileSize; }
            prop = prop->NextSiblingElement("property");
        }
        mDoors.emplace_back
        (
            Vector(mAttributeSpawn), 
            Vector(tileSize, tileSize), 
            target, 
            mRenderer, 
            mPlatformsTexturePath, 
            Vector(mSpawnPosition)
        );
    }
/*
-------------------------------------------------------------------------------
    Função de manipulação de eventos.
    Aqui é onde os eventos do teclado e do mouse são tratados.
    A função também lida com a alternância entre o modo de tela cheia e janela.
-------------------------------------------------------------------------------
*/
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
/*
-------------------------------------------------------------------------------
Função de atualização do jogo,
Aqui é onde a lógica do jogo é atualizada, incluindo a movimentação do jogador,
detecção de colisões e a atualização da câmera.
A função também lida com a transição entre os níveis.
-------------------------------------------------------------------------------
*/
    void Game::update()
    {
        handleTransition();
        if (!isTransitioning) 
        {
            updateGameState();
            updateCamera();
            updateCrates();
        }
    }

    void Game::handleTransition()
    {
        if (!isTransitioning) return;

    Vector currentVelocity = mPlayer.getVelocity();
    mPlayer.setVelocity(Vector(0, 0));
    
    if (SDL_GetTicks() - transitionStartTime > TRANSITION_DELAY) 
    {
        completeTransition(currentVelocity);
    }
    }

    void Game::completeTransition(const Vector& currentVelocity) 
    {
        loadGameLevelFromTMX(targetLevel);
        mPlayer.setPosition(targetSpawn);
        mPlayer.setVelocity(currentVelocity);
        mPlayerActivated = false;
        mActivationTime = SDL_GetTicks() + 500;
        isTransitioning = false;
    }

    void Game::updateGameState()
    {
        checkPlayerActivation();
        updatePlayer();
        checkLevelTransitions();
    }

    void Game::checkPlayerActivation()
    {
        if (SDL_GetTicks() > mActivationTime) { mPlayerActivated = true; }
    }

    void Game::updatePlayer() 
    {
        if (mPlayerActivated) 
        {
        mPlayer.update(deltaTime);
        PhysicsEngine::HandleCollisions(mPlayer, mWalls, mPlatforms, mSolidPlatforms);
        }
    }

    void Game::checkLevelTransitions()
    {
        std::string levelToLoad;
        Vector spawnPosition;
        if 
        (
            PhysicsEngine::HandlePlayerCollisions
            (
                mPlayer, 
                spawnPosition, 
                mDoors, 
                mCrates, 
                levelToLoad)
            ) 
            {
            startTransition(levelToLoad, spawnPosition);
            }
        }

    void Game::startTransition(const std::string& level, const Vector& spawn) 
    {
        if (!isTransitioning) 
        {
            isTransitioning = true;
            transitionStartTime = SDL_GetTicks();
            targetLevel = level;
            targetSpawn = spawn;
        }
    }

    void Game::updateCamera() 
    {
        Vector playerCenter = getPlayerCenter();
        Vector cameraPosition = calculateCameraPosition(playerCenter);
        cameraPosition.x = 
            std::max(0.0f, std::min(cameraPosition.x, 
            static_cast<float>(mapWidth) - effectiveScreenWidth));
        cameraPosition.y = 
            std::max(0.0f, std::min(cameraPosition.y, 
            static_cast<float>(mapHeight) - effectiveScreenHeight));
        applyCameraMargins(playerCenter, cameraPosition);
        mCamera.setPosition(cameraPosition);
    }

    Vector Game::getPlayerCenter() const 
    {
        Vector playerPosition = mPlayer.getPosition();
        return Vector
        (
        playerPosition.x + mPlayer.getSize().x / 2, 
        playerPosition.y + mPlayer.getSize().y / 2
        );
    }

    Vector Game::calculateCameraPosition(const Vector& playerCenter) const 
    {
        return Vector
        (
        playerCenter.x - effectiveScreenWidth / 2,
        playerCenter.y - effectiveScreenHeight / 2
        );
    }

    void Game::applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition) 
    {
        const float cameraMarginX = effectiveScreenWidth * 0.50f;
        const float cameraMarginY = effectiveScreenHeight * 0.50f;
        if 
        (playerCenter.x < cameraPosition.x + cameraMarginX) 
        {cameraPosition.x = playerCenter.x - cameraMarginX;} 
        else if 
        (playerCenter.x > cameraPosition.x + SCREEN_WIDTH - cameraMarginX) 
        {cameraPosition.x = playerCenter.x - (SCREEN_WIDTH - cameraMarginX);}
        if 
        (playerCenter.y < cameraPosition.y + cameraMarginY) 
        {cameraPosition.y = playerCenter.y - cameraMarginY;}
        else if 
        (playerCenter.y > cameraPosition.y + SCREEN_HEIGHT - cameraMarginY) 
        {cameraPosition.y = playerCenter.y - (SCREEN_HEIGHT - cameraMarginY);}
    }

    void Game::updateCrates() 
    {
        for (auto &crate : mCrates) 
        {
        crate.update(deltaTime);
        PhysicsEngine::HandleCollisions
        (
            crate, 
            mWalls, 
            mPlatforms, 
            mSolidPlatforms
        );
        }
    }
/*
-------------------------------------------------------------------------------
    Função de renderização do jogo,
    Aqui é onde todos os objetos do jogo são desenhados na tela, incluindo o fundo,
    os objetos do jogo e o jogador.
-------------------------------------------------------------------------------
*/
    void Game::render()
    {
        if (isTransitioning) 
        {
            renderTransitionEffect();
            return;
        }
        prepareRender();
        renderBackground();
        renderGameObjects();
        finalizeRender();
    }

    void Game::renderTransitionEffect() 
    {
        Uint32 elapsed = SDL_GetTicks() - transitionStartTime;
        if (elapsed <= HALF_TRANSITION) 
        {
        alpha = static_cast<int>(255 * (static_cast<float>(elapsed) / HALF_TRANSITION));
        }
        else { alpha = 0; }
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_Rect fadeRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(mRenderer, &fadeRect);
        SDL_RenderPresent(mRenderer);
    }

    void Game::prepareRender()
    {
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
        SDL_RenderClear(mRenderer);
    }

    void Game::renderBackground()
    {
        Vector cameraPos = mCamera.getPosition();
        for (int i = 0; i < 5; i++) 
        {
            if (mParallaxLayers[i]) 
            {
                renderParallaxLayer(i, cameraPos);
            }
        }
    }

    void Game::renderParallaxLayer(int layerIndex, const Vector& cameraPos) 
    {
        int texWidth, texHeight;
        SDL_QueryTexture(mParallaxLayers[layerIndex], nullptr, nullptr, &texWidth, &texHeight);
        float factor = mParallaxFactors[layerIndex];
        Vector parallaxOffset(cameraPos.x * factor, cameraPos.y * factor * 0.5f);
        Vector base(-parallaxOffset.x, -parallaxOffset.y);
        Vector repeat
        (
            static_cast<int>(effectiveScreenWidth / texWidth) + 2,
            static_cast<int>(effectiveScreenHeight / texHeight) + 2
        );
        Vector start
        (
        static_cast<int>(base.x) % texWidth,
        static_cast<int>(base.y) % texHeight
        );    
        if (start.x > 0) start.x -= texWidth;
        if (start.y > 0) start.y -= texHeight;
        for (int x = 0; x < repeat.x; x++) 
        {
            for (int y = 0; y < repeat.y; y++) 
            {
                SDL_Rect layerRect = 
                {
                    static_cast<int>(start.x + x * texWidth),
                    static_cast<int>(start.y + y * texHeight),
                    texWidth,
                    texHeight
                };
                SDL_RenderCopy(mRenderer, mParallaxLayers[layerIndex], nullptr, &layerRect);
            }
        }   
    }

    void Game::renderGameObjects() 
    {
        Vector cameraPos = mCamera.getPosition();
        Vector viewSize(effectiveScreenWidth, effectiveScreenHeight);
        renderObjects(mDecorations, cameraPos, viewSize);
        renderObjects(mPlatforms, cameraPos, viewSize);
        renderObjects(mSolidPlatforms, cameraPos, viewSize);
        renderObjects(mWalls, cameraPos, viewSize);
        renderObjects(mCrates, cameraPos, viewSize);
        renderObjects(mDoors, cameraPos, viewSize); 
        mPlayer.render(mRenderer, cameraPos);
    }

    template<typename T>

    void Game::renderObjects
    (
        std::list<T>& objects, 
        Vector& cameraPos, 
        Vector& viewSize
    ) 
    {
        for (auto& obj : objects) 
        {
            if (obj.isVisible(cameraPos, viewSize)) 
            { obj.render(mRenderer, cameraPos); }
        }
    }
    void Game::finalizeRender() { SDL_RenderPresent(mRenderer); }

    void Game::resetGame()
    {
        mPlayer.setVelocity(Vector(0, 0));   // Velocidade inicial
        mCamera.setPosition(Vector(0, 0));
        std::cout << "Resetting game..." << std::endl;
    }

    bool Game::isRunning(){return !mQuit;}

} // namespace BRTC