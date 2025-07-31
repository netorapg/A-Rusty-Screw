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
        , mAttackSound(nullptr)
        , mTooltipSound(nullptr)
        , mPlayerHitSound(nullptr)
        , mEnemyHitSound(nullptr)
        , mEnemyDeathSound(nullptr)
        , mPunktauroAccelerateSound(nullptr)
        , mPunktauroJumpSound(nullptr)
        , mPunktauroDeathSound(nullptr)
        , mGateSound(nullptr)
        , mWorld(renderer)
        , mHUD(renderer)
        , mQuit(false)
        , mIsRunning(true)
        , mPlayerActivated(false)
        , mBossDefeated(false)
        , mActivationTime(0)
        , mBossDefeatedTransition(false)
        , mBossDefeatedTime(0)
        , mCreditsFadeAlpha(0.0f)
        , isTransitioning(false)
        , transitionStartTime(0)
        , increasing(true)
        , alpha(0)
        , effectiveScreenWidth(SCREEN_WIDTH / PLAYER_ZOOM_FACTOR)
        , effectiveScreenHeight(SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR)
        , mCurrentState(GameState::MENU)
        , mGameOverScreen(renderer)
        , mStartMenu(renderer)
        , mPauseMenu(renderer)
        , mCreditsScreen(renderer)
{
    resetScore(); // Corrige score "quebrado" ao iniciar
    std::cout << "GameManager constructor called" << std::endl;
    initializeRenderSettings();
    loadParallaxLayers();
    initializeAudioSystem();
    // Configurar callbacks de som no GameWorld
    mWorld.setAttackSoundCallback([this]() { playAttackSound(); });
    mWorld.setTooltipSoundCallback([this]() { playTooltipSound(); });
    mWorld.setPlayerHitSoundCallback([this]() { playPlayerHitSound(); });
    mWorld.setEnemyHitSoundCallback([this]() { playEnemyHitSound(); });
    mWorld.setEnemyDeathSoundCallback([this]() { 
        playEnemyDeathSound();
        addEnemyKill(); // Atualiza score ao matar inimigo
    });
    mWorld.setPunktauroAccelerateSoundCallback([this]() { playPunktauroAccelerateSound(); });
    mWorld.setPunktauroJumpSoundCallback([this]() { playPunktauroJumpSound(); });
    mWorld.setPunktauroDeathSoundCallback([this]() { playPunktauroDeathSound(); });
    mWorld.setGateSoundCallback([this]() { playGateSound(); });
    // Iniciar música de fundo
    if (mMusic)
    {
        Mix_PlayMusic(mMusic, -1); // -1 = loop infinito
        Mix_VolumeMusic(64); // Volume 50% (0-128)
    }
    // Não carregar nível aqui - será carregado quando o player escolher "START GAME"
    // O jogo começa no menu
    mCurrentLevel = "";
    mPlayerActivated = false;
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
        Mix_FreeChunk(mAttackSound);
        Mix_FreeChunk(mTooltipSound);
        Mix_FreeChunk(mPlayerHitSound);
        Mix_FreeChunk(mEnemyHitSound);
        Mix_FreeChunk(mEnemyDeathSound);
        Mix_FreeChunk(mPunktauroAccelerateSound);
        Mix_FreeChunk(mPunktauroJumpSound);
        Mix_FreeChunk(mPunktauroDeathSound);
        Mix_FreeChunk(mGateSound);
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
        
        // Aumentar o número de canais de áudio disponíveis
        Mix_AllocateChannels(32); // Aumentar de 8 (padrão) para 32 canais
        
        // Carregar música de fundo
        mMusic = Mix_LoadMUS("../assets/backgroundsong.mp3");
        if (!mMusic)
        {
            std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de pulo
        mJumpSound = Mix_LoadWAV("../assets/jump-sound.wav");
        if (!mJumpSound)
        {
            std::cerr << "Failed to load jump sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de ataque
        mAttackSound = Mix_LoadWAV("../assets/clank.wav");
        if (!mAttackSound)
        {
            std::cerr << "Failed to load attack sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de tooltip
        mTooltipSound = Mix_LoadWAV("../assets/found-upgrade.wav");
        if (!mTooltipSound)
        {
            std::cerr << "Failed to load tooltip sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de jogador atingido
        mPlayerHitSound = Mix_LoadWAV("../assets/hurt-sound.wav");
        if (!mPlayerHitSound)
        {
            std::cerr << "Failed to load player hit sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de inimigo atingido
        mEnemyHitSound = Mix_LoadWAV("../assets/enemy-hitted.wav");
        if (!mEnemyHitSound)
        {
            std::cerr << "Failed to load enemy hit sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de morte do inimigo
        mEnemyDeathSound = Mix_LoadWAV("../assets/enemy-dead.wav");
        if (!mEnemyDeathSound)
        {
            std::cerr << "Failed to load enemy death sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de aceleração do Punktauro
        mPunktauroAccelerateSound = Mix_LoadWAV("../assets/accelerate.wav");
        if (!mPunktauroAccelerateSound)
        {
            std::cerr << "Failed to load Punktauro accelerate sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de pulo do Punktauro
        mPunktauroJumpSound = Mix_LoadWAV("../assets/boss-jump.wav");
        if (!mPunktauroJumpSound)
        {
            std::cerr << "Failed to load Punktauro jump sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som de morte do Punktauro
        mPunktauroDeathSound = Mix_LoadWAV("../assets/boss-explode.wav");
        if (!mPunktauroDeathSound)
        {
            std::cerr << "Failed to load Punktauro death sound: " << Mix_GetError() << std::endl;
        }
        
        // Carregar som do gate
        mGateSound = Mix_LoadWAV("../assets/gate-sound.wav");
        if (!mGateSound)
        {
            std::cerr << "Failed to load gate sound: " << Mix_GetError() << std::endl;
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
            
            // Processar eventos baseado no estado atual
            switch (mCurrentState)
            {
                case GameState::MENU:
                    // Processar input do menu de start
                    mStartMenu.handleInput(e);
                    break;
                    
                case GameState::PLAYING:
                    // Delegar eventos para o GameWorld apenas quando jogando
                    mWorld.getInputManager().handleEvent(e);

                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
                        resetGame();

                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                        switchToPaused();

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
                    break;
                    
                case GameState::PAUSED:
                    // Processar apenas input do menu de pausa
                    mPauseMenu.handleInput(e);
                    break;
                    
                case GameState::GAME_OVER:
                    // Processar apenas input da tela de game over
                    mGameOverScreen.handleInput(e);
                    break;
                    
                case GameState::CREDITS:
                    // Processar apenas input da tela de créditos
                    mCreditsScreen.handleInput(e);
                    break;
            }
        }
    }

    void GameManager::update(float deltaTime)
    {
        switch (mCurrentState)
        {
            case GameState::MENU:
                updateMenu(deltaTime);
                break;
            case GameState::PLAYING:
                updatePlaying(deltaTime);
                updateGameTime(deltaTime);
                updateAirTime(deltaTime, !mWorld.getPlayer().isOnGround());
                break;
            case GameState::PAUSED:
                updatePaused(deltaTime);
                break;
            case GameState::GAME_OVER:
                updateGameOver(deltaTime);
                addGameOver();
                break;
            case GameState::CREDITS:
                updateCredits(deltaTime);
                break;
        }
    }

    void GameManager::updatePlaying(float deltaTime)
    {
        handleTransition();
        if (!isTransitioning)
        {
            updateGameState();
            updateCamera();
            mWorld.updateWorld(deltaTime);
            mHUD.update(deltaTime);
        }
        
        // Verificar se o jogador morreu
        if (mWorld.getPlayer().isDead())
        {
            switchToGameOver();
        }
        
        // Verificar se o Punktauro foi derrotado (apenas uma vez)
        if (!mBossDefeated && mWorld.getPunktauro() && mWorld.getPunktauro()->isDead())
        {
            std::cout << "=== BOSS DERROTADO! INICIANDO TRANSIÇÃO PARA CRÉDITOS ===" << std::endl;
            mBossDefeated = true;
            mBossDefeatedTransition = true;
            mBossDefeatedTime = SDL_GetTicks();
            mCreditsFadeAlpha = 0.0f;
        }
        
        // Verificar transição para créditos após boss derrotado
        if (mBossDefeatedTransition)
        {
            updateBossDefeatedTransition();
        }
    }

    void GameManager::updateGameOver(float deltaTime)
    {
        mGameOverScreen.update(deltaTime);
        
        if (mGameOverScreen.isOptionConfirmed())
        {
            if (mGameOverScreen.getSelectedOption() == GameOverOption::RESTART)
            {
                restartGame();
            }
            else // QUIT
            {
                mQuit = true;
            }
        }
    }

    void GameManager::updateGameState()
    {
        checkPlayerActivation();
        updatePlayer();
        checkLevelTransitions();
        mWorld.handleScrewCollisions();
        mWorld.handleEnemyCollisions();
        mWorld.handlePunktauroCollisions();

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
                mWorld.getSolidPlatforms()
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
        mCurrentLevel = targetLevel;
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
        // Não limpar nem apresentar aqui - o main.cpp é responsável por isso
        
        switch (mCurrentState)
        {
            case GameState::MENU:
                renderMenu();
                break;
            case GameState::PLAYING:
                renderPlaying();
                break;
            case GameState::PAUSED:
                renderPaused();
                break;
            case GameState::GAME_OVER:
                renderGameOver();
                break;
            case GameState::CREDITS:
                renderCredits();
                break;
        }
    }

    void GameManager::renderPlaying()
    {
        renderBackground();
        
        Vector cameraPos = mWorld.getCamera().getPosition();
        Vector viewSize(effectiveScreenWidth, effectiveScreenHeight);
        Vector snappedCameraPos(std::floor(cameraPos.x), std::floor(cameraPos.y));
        mWorld.renderWorld(mRenderer, snappedCameraPos, viewSize);
        renderHUD();
        
        // Renderizar efeito de transição se ativo
        if (isTransitioning)
        {
            renderTransitionEffect();
        }
        
        // Renderizar efeito de fade dos créditos se ativo
        renderCreditsFadeEffect();
    }

    void GameManager::renderGameOver()
    {
        // Renderizar o jogo atrás (pausado) COM escala
        renderWorld();
        renderHUD();
        
        // Resetar escala para renderizar a tela de game over em resolução nativa
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        
        // Renderizar a tela de game over por cima (sem escala)
        mGameOverScreen.render(mRenderer);
        
        // Restaurar escala original
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    }

    void GameManager::renderHUD()
    {
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        // Score parcial: só inimigos mortos e tempo no ar
        float partialScore = mEnemiesKilled * 100 + mBestAirTime * 50;
        mHUD.setScore(partialScore);
        mHUD.render(mRenderer, mWorld.getPlayer(), mWorld.getPunktauro());
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
        // Não chamar SDL_RenderPresent aqui - o main.cpp é responsável
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
            int channel = Mix_PlayChannel(-1, mJumpSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play jump sound: " << Mix_GetError() << std::endl;
                
                // Tentar limpar canais automaticamente e reproduzir novamente
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mJumpSound, 0);
                if (channel != -1) {
                    std::cout << "Jump sound played successfully after cleanup" << std::endl;
                } else {
                    std::cerr << "Still failed to play jump sound after cleanup" << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Jump sound not loaded!" << std::endl;
        }
    }

    void GameManager::playAttackSound()
    {
        if (mAttackSound)
        {
            int channel = Mix_PlayChannel(-1, mAttackSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play attack sound: " << Mix_GetError() << std::endl;
                
                // Tentar limpar canais automaticamente e reproduzir novamente
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mAttackSound, 0);
                if (channel != -1) {
                    std::cout << "Attack sound played successfully after cleanup" << std::endl;
                } else {
                    std::cerr << "Still failed to play attack sound after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playTooltipSound()
    {
        if (mTooltipSound)
        {
            int channel = Mix_PlayChannel(-1, mTooltipSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play tooltip sound: " << Mix_GetError() << std::endl;
                
                // Tentar limpar canais automaticamente e reproduzir novamente
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mTooltipSound, 0);
                if (channel != -1) {
                    std::cout << "Tooltip sound played successfully after cleanup" << std::endl;
                } else {
                    std::cerr << "Still failed to play tooltip sound after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playPlayerHitSound()
    {
        if (mPlayerHitSound)
        {
            int channel = Mix_PlayChannel(-1, mPlayerHitSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play player hit sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mPlayerHitSound, 0);
                if (channel != -1) {
                    std::cout << "Player hit sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playEnemyHitSound()
    {
        if (mEnemyHitSound)
        {
            int channel = Mix_PlayChannel(-1, mEnemyHitSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play enemy hit sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mEnemyHitSound, 0);
                if (channel != -1) {
                    std::cout << "Enemy hit sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playEnemyDeathSound()
    {
        if (mEnemyDeathSound)
        {
            int channel = Mix_PlayChannel(-1, mEnemyDeathSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play enemy death sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mEnemyDeathSound, 0);
                if (channel != -1) {
                    std::cout << "Enemy death sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playPunktauroAccelerateSound()
    {
        if (mPunktauroAccelerateSound)
        {
            int channel = Mix_PlayChannel(-1, mPunktauroAccelerateSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play Punktauro accelerate sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mPunktauroAccelerateSound, 0);
                if (channel != -1) {
                    std::cout << "Punktauro accelerate sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playPunktauroJumpSound()
    {
        if (mPunktauroJumpSound)
        {
            int channel = Mix_PlayChannel(-1, mPunktauroJumpSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play Punktauro jump sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mPunktauroJumpSound, 0);
                if (channel != -1) {
                    std::cout << "Punktauro jump sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playPunktauroDeathSound()
    {
        if (mPunktauroDeathSound)
        {
            int channel = Mix_PlayChannel(-1, mPunktauroDeathSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play Punktauro death sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mPunktauroDeathSound, 0);
                if (channel != -1) {
                    std::cout << "Punktauro death sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::playGateSound()
    {
        if (mGateSound)
        {
            int channel = Mix_PlayChannel(-1, mGateSound, 0);
            if (channel == -1)
            {
                std::cerr << "Failed to play gate sound: " << Mix_GetError() << std::endl;
                cleanupAudioChannels();
                channel = Mix_PlayChannel(-1, mGateSound, 0);
                if (channel != -1) {
                    std::cout << "Gate sound played successfully after cleanup" << std::endl;
                }
            }
        }
    }

    void GameManager::switchToGameOver()
    {
        mCurrentState = GameState::GAME_OVER;
        mGameOverScreen.reset();
        mGameOverScreen.startFadeIn();
    }

    void GameManager::restartGame()
    {
        // Recarregar o nível atual
        mWorld.loadLevelFromTMX(mCurrentLevel);

        // Restaurar a vida do player ao máximo
        mWorld.getPlayer().resetHealth();

        // Resetar score ao reiniciar
        resetScore();

        // Resetar flag do boss derrotado
        mBossDefeated = false;
        mBossDefeatedTransition = false;
        mBossDefeatedTime = 0;
        mCreditsFadeAlpha = 0.0f;

        mCurrentState = GameState::PLAYING;
        mGameOverScreen.reset();
    }

    bool GameManager::isRunning() { return !mQuit; }

    void GameManager::renderWorld()
    {
        renderBackground();
        
        Vector cameraPos = mWorld.getCamera().getPosition();
        Vector viewSize(effectiveScreenWidth, effectiveScreenHeight);
        Vector snappedCameraPos(std::floor(cameraPos.x), std::floor(cameraPos.y));
        mWorld.renderWorld(mRenderer, snappedCameraPos, viewSize);
    }

    void GameManager::updateMenu(float deltaTime)
    {
        mStartMenu.update(deltaTime);
        
        if (mStartMenu.isOptionConfirmed())
        {
            if (mStartMenu.getSelectedOption() == StartMenuOption::START_GAME)
            {
                switchToPlaying();
            }
            else // QUIT
            {
                mQuit = true;
            }
        }
    }

    void GameManager::renderMenu()
    {
        // Resetar escala para renderizar o menu em resolução nativa
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        
        // Renderizar o menu de start
        mStartMenu.render(mRenderer);
        
        // Restaurar escala original (não necessário aqui mas por consistência)
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    }

    void GameManager::updatePaused(float deltaTime)
    {
        mPauseMenu.update(deltaTime);
        
        if (mPauseMenu.isOptionConfirmed())
        {
            switch (mPauseMenu.getSelectedOption())
            {
                case PauseMenuOption::RESUME:
                    switchToPlaying();
                    break;
                case PauseMenuOption::RESTART:
                    restartGame();
                    break;
                case PauseMenuOption::MAIN_MENU:
                    switchToMenu();
                    break;
                case PauseMenuOption::QUIT:
                    mQuit = true;
                    break;
            }
        }
    }

    void GameManager::renderPaused()
    {
        // Renderizar o jogo atrás (pausado) COM escala
        renderWorld();
        renderHUD();
        
        // Resetar escala para renderizar o menu de pausa em resolução nativa
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        
        // Renderizar o menu de pausa por cima (sem escala)
        mPauseMenu.render(mRenderer);
        
        // Restaurar escala original
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    }

    void GameManager::switchToPaused()
    {
        mCurrentState = GameState::PAUSED;
        mPauseMenu.show();
    }

    void GameManager::switchToMenu()
    {
        mCurrentState = GameState::MENU;
        mPauseMenu.hide();
        mStartMenu.reset();
        
        // Limpar o nível atual para forçar reload quando voltar
        mCurrentLevel = "";
    }

    void GameManager::switchToPlaying()
    {
        mCurrentState = GameState::PLAYING;
        mPauseMenu.hide();
        mStartMenu.reset();
        
        // Resetar flag do boss derrotado ao começar novo jogo
        mBossDefeated = false;
        mBossDefeatedTransition = false;
        mBossDefeatedTime = 0;
        mCreditsFadeAlpha = 0.0f;
        
        // Carregar nível inicial se necessário
        if (mCurrentLevel.empty()) {
            mCurrentLevel = "../map/demoroom1.tmx";
            mWorld.loadLevelFromTMX(mCurrentLevel);
            mPlayerActivated = false;
            mActivationTime = SDL_GetTicks() + 500;
            centerCameraOnPlayer();
        }
    }

    std::string GameManager::getCurrentLevelPath() const
    {
        return mCurrentLevel;
    }

    void GameManager::switchToCredits()
    {
        mCurrentState = GameState::CREDITS;
        mCreditsScreen.reset();
        mCreditsScreen.startCredits();
        std::cout << "Transitioning to credits screen..." << std::endl;
    }

    void GameManager::updateCredits(float deltaTime)
    {
        mCreditsScreen.update(deltaTime);
        
        if (mCreditsScreen.isOptionConfirmed())
        {
            if (mCreditsScreen.getSelectedOption() == CreditsOption::MAIN_MENU)
            {
                switchToMenu();
            }
            else // QUIT
            {
                mQuit = true;
            }
        }
    }

    void GameManager::addEnemyKill() {
        mEnemiesKilled++;
    }

  
    void GameManager::addGameOver() {
        mGameOverCount++;
    }

  
    void GameManager::addDamageTaken(float dmg) {
        mTotalDamageTaken += dmg;
    }


    void GameManager::updateAirTime(float deltaTime, bool inAir) {
        if (inAir) {
            mCurrentAirTime += deltaTime;
            if (mCurrentAirTime > mBestAirTime)
                mBestAirTime = mCurrentAirTime;
        } else {
            mCurrentAirTime = 0.0f;
        }
    }

  
    void GameManager::updateGameTime(float deltaTime) {
        mGameTime += deltaTime;
    }


    float GameManager::calculateFinalScore() const {
        float score = 0;
        score += mEnemiesKilled * 100;                
        score += mBestAirTime * 50;                    
        score += std::max(0.0f, 5000.0f - mGameTime);
        score -= mGameOverCount * 200;
        score -= mTotalDamageTaken;
        return std::max(0.0f, score);
    }

    // Chame ao iniciar/reiniciar o jogo
    void GameManager::resetScore() {
        mScore = 0;
        mEnemiesKilled = 0;
        mGameOverCount = 0;
        mTotalDamageTaken = 0;
        mBestAirTime = 0.0f;
        mCurrentAirTime = 0.0f;
        mGameTime = 0.0f;
    }

    void GameManager::renderCredits()
    {
        // Resetar escala para renderizar os créditos em resolução nativa
        SDL_RenderSetScale(mRenderer, 1.0f, 1.0f);
        
        // Renderizar a tela de créditos
        mCreditsScreen.render(mRenderer);
        
        // Restaurar escala original
        SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    }

    void GameManager::renderCreditsFadeEffect()
    {
        if (mBossDefeatedTransition && mCreditsFadeAlpha > 0.0f)
        {
            SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
            SDL_Rect fadeRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, static_cast<Uint8>(mCreditsFadeAlpha));
            SDL_RenderFillRect(mRenderer, &fadeRect);
        }
    }

    void GameManager::updateBossDefeatedTransition()
    {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - mBossDefeatedTime;
        
        if (elapsedTime >= BOSS_DEFEATED_DELAY)
        {
            // Transição completa, ir para créditos
            mBossDefeatedTransition = false;
            switchToCredits();
        }
        else
        {
            // Calcular o fade baseado no tempo decorrido
            // Começar o fade depois de 1 segundo
            if (elapsedTime > 1000)
            {
                float fadeTime = (elapsedTime - 1000) / 2000.0f; // Fade em 2 segundos
                mCreditsFadeAlpha = std::min(255.0f, fadeTime * 255.0f);
            }
        }
    }

    void GameManager::cleanupAudioChannels()
    {
        // Verificar quantos canais estão sendo usados
        int totalChannels = Mix_AllocateChannels(-1);
        int playingChannels = 0;
        
        for (int i = 0; i < totalChannels; i++) {
            if (Mix_Playing(i)) {
                playingChannels++;
            }
        }
        
        // Se muitos canais estão sendo usados, parar alguns dos mais antigos
        if (playingChannels > totalChannels * 0.8f) { // Se mais de 80% dos canais estão em uso
            std::cout << "Too many audio channels in use (" << playingChannels << "/" << totalChannels 
                      << "), cleaning up..." << std::endl;
            
            // Parar metade dos canais para liberar espaço
            for (int i = 0; i < totalChannels / 2; i++) {
                if (Mix_Playing(i)) {
                    Mix_HaltChannel(i);
                }
            }
        }
    }
}