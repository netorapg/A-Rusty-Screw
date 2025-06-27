#include "../include/arscrew/GameManager.h"
#include "../include/arscrew/CollisionEngine.h"
#include <iostream>
#include <algorithm>

namespace ARSCREW
{
    GameManager::GameManager(SDL_Window* window, SDL_Renderer* renderer)
        : mWindow(window)
        , mRenderer(renderer)
        , mFont(nullptr)
        , mSmallFont(nullptr)
        , mMusic(nullptr)
        , mJumpSound(nullptr)
        , mWorld(renderer)
        , mHUD(renderer)
        , mQuit(false)
        , mPlayerActivated(false)
        , mActivationTime(0)
        , isTransitioning(false)
        , transitionStartTime(0)
        , increasing(true)
        , alpha(0)
        , effectiveScreenWidth(SCREEN_WIDTH / PLAYER_ZOOM_FACTOR)
        , effectiveScreenHeight(SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR)
    {
        std::cout << "GameManager constructor called" << std::endl;
        initializeRenderSettings();
        loadParallaxLayers();
        initializeAudioSystem();
        
        // Iniciar música de fundo
        if (mMusic)
        {
            Mix_PlayMusic(mMusic, -1); // -1 = loop infinito
            Mix_VolumeMusic(64); // Volume 50% (0-128)
        }
        
        // Carregar nível inicial
        mWorld.loadLevelFromTMX("../map/mapabanca1.tmx");
        mPlayerActivated = false;
        mActivationTime = SDL_GetTicks() + 500;
        centerCameraOnPlayer();
    }

    GameManager::~GameManager()
    {
        for (int i = 0; i < 5; i++)
        {
            if (mParallaxLayers[i]) SDL_DestroyTexture(mParallaxLayers[i]);
        }
        
        // Liberar recursos de áudio
        if (mMusic)
        {
            Mix_FreeMusic(mMusic);
            mMusic = nullptr;
        }
        Mix_FreeChunk(mJumpSound);
        Mix_CloseAudio();
        
        TTF_CloseFont(mFont);
        TTF_CloseFont(mSmallFont);
        TTF_Quit();
    }

    void GameManager::initializeRenderSettings()
    {
        SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
    }

    void GameManager::initializeAudioSystem()
    {
        if (TTF_Init() == -1)
        {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
        
        // Carregar música de fundo
        mMusic = Mix_LoadMUS("");
        if (!mMusic)
        {
            std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de pulo
        mJumpSound = Mix_LoadWAV("../assets/jump.wav");
        if (!mJumpSound)
        {
            std::cerr << "Failed to load jump sound: " << Mix_GetError() << std::endl;
        }
    }

    void GameManager::loadParallaxLayers()
    {
        const std::array<const char*, 5> layerFiles = {
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

    void GameManager::centerCameraOnPlayer()
    {
        Vector playerPos = mWorld.getPlayer().getPosition();
        mWorld.getCamera().setPosition(Vector(
            playerPos.x - (SCREEN_WIDTH / (2 * PLAYER_ZOOM_FACTOR)),
            playerPos.y - (SCREEN_HEIGHT / (2 * PLAYER_ZOOM_FACTOR))
        ));
    }

    void GameManager::handleEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                mQuit = true;
            
            // Delegar eventos para o GameWorld
            mWorld.getInputManager().handleEvent(e);

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
                resetGame();

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11)
            {
                Uint32 flags = SDL_GetWindowFlags(mWindow);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                    SDL_SetWindowFullscreen(mWindow, 0);
                else
                    SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F1)
            {
                mHUD.setVisible(!mHUD.isVisible());
            }
        }
    }

    void GameManager::update()
    {
        handleTransition();
        if (!isTransitioning)
        {
            updateGameState();
            updateCamera();
            mWorld.updateWorld(deltaTime);
            mHUD.update(deltaTime);
        }
    }

    void GameManager::updateGameState()
    {
        checkPlayerActivation();
        updatePlayer();
        checkLevelTransitions();
        mWorld.handleScrewCollisions();
        mWorld.handleEnemyCollisions();
    }

    void GameManager::checkPlayerActivation()
    {
        if (SDL_GetTicks() > mActivationTime) mPlayerActivated = true;
    }

    void GameManager::updatePlayer()
    {
        if (mPlayerActivated)
        {
            static bool wasJumping = false; // Flag para controlar se já tocou o som
            
            // Verificar se o jogador está no chão antes da atualização
            bool wasOnGround = mWorld.getPlayer().isOnGround();
            Vector oldVelocity = mWorld.getPlayer().getVelocity();
            
            mWorld.getPlayer().update(deltaTime);
            
            // Verificar se o jogador está no chão após a atualização
            bool isOnGround = mWorld.getPlayer().isOnGround();
            Vector newVelocity = mWorld.getPlayer().getVelocity();
            
            // Se estava no chão e agora não está (começou a pular) E não estava pulando antes
            if (wasOnGround && newVelocity.y < 0 && !wasJumping)
            {
                std::cout << "Jump detected! Playing jump sound..." << std::endl;
                playJumpSound();
                wasJumping = true; // Marca que já tocou o som
            }
            
            // Reset da flag quando volta ao chão
            if (isOnGround)
            {
                wasJumping = false;
            }
            
            CollisionEngine::HandleCollisions(
                mWorld.getPlayer(),
                mWorld.getPlatforms(),
                mWorld.getSolidPlatforms(),
                mWorld.getRamps()
            );
        }
    }

    void GameManager::checkLevelTransitions()
    {
        std::string levelToLoad;
        Vector spawnPosition;
        if (CollisionEngine::HandlePlayerCollisions(
            mWorld.getPlayer(),
            spawnPosition,
            mWorld.getDoors(),
            mWorld.getCrates(),
            levelToLoad))
        {
            startTransition(levelToLoad, spawnPosition);
        }
    }

    void GameManager::startTransition(const std::string& level, const Vector& spawn)
    {
        if (!isTransitioning)
        {
            isTransitioning = true;
            transitionStartTime = SDL_GetTicks();
            targetLevel = level;
            targetSpawn = spawn;
        }
    }

    void GameManager::handleTransition()
    {
        if (!isTransitioning) return;

        Vector currentVelocity = mWorld.getPlayer().getVelocity();
        mWorld.getPlayer().setVelocity(Vector(0, 0));

        if (SDL_GetTicks() - transitionStartTime > TRANSITION_DELAY)
        {
            completeTransition(currentVelocity);
        }
    }

    void GameManager::completeTransition(const Vector& currentVelocity)
    {
        mWorld.loadLevelFromTMX(targetLevel);
        mWorld.getPlayer().setPosition(targetSpawn);
        mWorld.getPlayer().setVelocity(currentVelocity);
        mPlayerActivated = false;
        mActivationTime = SDL_GetTicks() + 500;
        isTransitioning = false;
    }

    void GameManager::updateCamera()
    {
        Vector playerCenter = getPlayerCenter();
        Vector desiredPosition = calculateCameraPosition(playerCenter);

        desiredPosition.x = std::max(0.0f, std::min(desiredPosition.x, mWorld.getMapWidth() - effectiveScreenWidth));
        desiredPosition.y = std::max(0.0f, std::min(desiredPosition.y, mWorld.getMapHeight() - effectiveScreenHeight));

        applyCameraMargins(playerCenter, desiredPosition);

        Vector currentPos = mWorld.getCamera().getPosition();
        Vector newPosition = Vector(
            currentPos.x + (desiredPosition.x - currentPos.x) * mCameraSmoothSpeed * deltaTime,
            currentPos.y + (desiredPosition.y - currentPos.y) * mCameraSmoothSpeed * deltaTime
        );
        mWorld.getCamera().setPosition(newPosition);
    }

    Vector GameManager::getPlayerCenter() const
    {
        Vector playerPosition = mWorld.getPlayer().getPosition();
        return Vector(
            playerPosition.x + mWorld.getPlayer().getSize().x / 2,
            playerPosition.y + mWorld.getPlayer().getSize().y / 2
        );
    }

    Vector GameManager::calculateCameraPosition(const Vector& playerCenter) const
    {
        return Vector(
            playerCenter.x - effectiveScreenWidth / 2,
            playerCenter.y - effectiveScreenHeight / 2
        );
    }

    void GameManager::applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition)
    {
        const float cameraMarginX = effectiveScreenWidth * 0.50f;
        const float cameraMarginY = effectiveScreenHeight * 0.50f;
        
        if (playerCenter.x < cameraPosition.x + cameraMarginX)
            cameraPosition.x = playerCenter.x - cameraMarginX;
        else if (playerCenter.x > cameraPosition.x + SCREEN_WIDTH - cameraMarginX)
            cameraPosition.x = playerCenter.x - (SCREEN_WIDTH - cameraMarginX);
            
        if (playerCenter.y < cameraPosition.y + cameraMarginY)
            cameraPosition.y = playerCenter.y - cameraMarginY;
        else if (playerCenter.y > cameraPosition.y + SCREEN_HEIGHT - cameraMarginY)
            cameraPosition.y = playerCenter.y - (SCREEN_HEIGHT - cameraMarginY);
    }

    void GameManager::render()
    {
        if (isTransitioning)
        {
            renderTransitionEffect();
            return;
        }
        prepareRender();
        renderBackground();
        
        Vector cameraPos = mWorld.getCamera().getPosition();
        Vector viewSize(effectiveScreenWidth, effectiveScreenHeight);
        Vector snappedCameraPos(std::floor(cameraPos.x), std::floor(cameraPos.y));
        mWorld.renderWorld(mRenderer, snappedCameraPos, viewSize);
        renderHUD();
        finalizeRender();
    }

    void GameManager::renderHUD()
    {
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        mHUD.render(mRenderer, mWorld.getPlayer());
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    }

    void GameManager::renderTransitionEffect()
    {
        Uint32 elapsed = SDL_GetTicks() - transitionStartTime;
        if (elapsed <= HALF_TRANSITION)
        {
            alpha = static_cast<int>(255 * (static_cast<float>(elapsed) / HALF_TRANSITION));
        }
        else
        {
            alpha = 0;
        }
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_Rect fadeRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(mRenderer, &fadeRect);
        SDL_RenderPresent(mRenderer);
    }

    void GameManager::prepareRender()
    {
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
        SDL_RenderClear(mRenderer);
    }

    void GameManager::renderBackground()
    {
        Vector cameraPos = mWorld.getCamera().getPosition();
        for (int i = 0; i < 5; i++)
        {
            if (mParallaxLayers[i])
            {
                renderParallaxLayer(i, cameraPos);
            }
        }
    }

    void GameManager::renderParallaxLayer(int layerIndex, const Vector& cameraPos)
    {
        int texWidth, texHeight;
        SDL_QueryTexture(mParallaxLayers[layerIndex], nullptr, nullptr, &texWidth, &texHeight);
        float factor = mParallaxFactors[layerIndex];
        Vector parallaxOffset(cameraPos.x * factor, cameraPos.y * factor * 0.5f);
        Vector base(-parallaxOffset.x, -parallaxOffset.y);
        Vector repeat(
            static_cast<int>(effectiveScreenWidth / texWidth) + 2,
            static_cast<int>(effectiveScreenHeight / texHeight) + 2
        );
        Vector start(
            static_cast<int>(base.x) % texWidth,
            static_cast<int>(base.y) % texHeight
        );
        
        if (start.x > 0) start.x -= texWidth;
        if (start.y > 0) start.y -= texHeight;
        
        for (int x = 0; x < repeat.x; x++)
        {
            for (int y = 0; y < repeat.y; y++)
            {
                SDL_Rect layerRect = {
                    static_cast<int>(start.x + x * texWidth),
                    static_cast<int>(start.y + y * texHeight),
                    texWidth,
                    texHeight
                };
                SDL_RenderCopy(mRenderer, mParallaxLayers[layerIndex], nullptr, &layerRect);
            }
        }
    }

    void GameManager::finalizeRender() { /* SDL_RenderPresent será chamado no main */ }

    void GameManager::resetGame()
    {
        mWorld.getPlayer().setVelocity(Vector(0, 0));
        mWorld.getCamera().setPosition(Vector(0, 0));
        std::cout << "Resetting game..." << std::endl;
    }

    void GameManager::playJumpSound()
    {
        if (mJumpSound)
        {
            std::cout << "Attempting to play jump sound..." << std::endl;
            int channel = Mix_PlayChannel(-1, mJumpSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play jump sound: " << Mix_GetError() << std::endl;
            }
            else
            {
                std::cout << "Jump sound playing on channel: " << channel << std::endl;
            }
        }
        else
        {
            std::cout << "Jump sound not loaded!" << std::endl;
        }
    }

    bool GameManager::isRunning() { return !mQuit; }
}