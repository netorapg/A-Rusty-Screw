#include "../include/bettlerider/GameManager.h"

namespace BRTC {

GameManager::GameManager(SDL_Window* window, SDL_Renderer* renderer) :
    mWindow(window),
    mRenderer(renderer),
    mGameWorld(renderer),
    mQuit(false),
    mPlayerActivated(false),
    mActivationTime(SDL_GetTicks() + 500),
    isTransitioning(false),
    increasing(true),
    alpha(0),
    mFont(nullptr),
    mSmallFont(nullptr),
    mMusic(nullptr),
    mJumpSound(nullptr)
{
    initializeAudioSystem();
}

GameManager::~GameManager()
{
    Mix_FreeChunk(mJumpSound);
    TTF_CloseFont(mFont);
    TTF_CloseFont(mSmallFont);
    TTF_Quit();
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
}

void GameManager::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            mQuit = true;
        
        mGameWorld.getPlayer().handleEvent(e);

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
            resetGame();

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
        {
            // Mix_PlayChannel(-1, mJumpSound, 0);
        }

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

void GameManager::update()
{
    handleTransition();
    if (!isTransitioning) 
    {
        updateGameState();
        mGameWorld.updateCamera();
        mGameWorld.updateCrates();
    }
}

void GameManager::render()
{
    if (isTransitioning) 
    {
        renderTransitionEffect();
        return;
    }
    prepareRender();
    mGameWorld.renderBackground();
    mGameWorld.renderGameObjects();
    finalizeRender();
}

void GameManager::handleTransition()
{
    if (!isTransitioning) return;

    Vector currentVelocity = mGameWorld.getPlayer().getVelocity();
    mGameWorld.getPlayer().setVelocity(Vector(0, 0));
    
    if (SDL_GetTicks() - transitionStartTime > TRANSITION_DELAY) 
    {
        completeTransition(currentVelocity);
    }
}

void GameManager::completeTransition(const Vector& currentVelocity) 
{
    mGameWorld.loadGameLevelFromTMX(targetLevel);
    mGameWorld.getPlayer().setPosition(targetSpawn);
    mGameWorld.getPlayer().setVelocity(currentVelocity);
    mPlayerActivated = false;
    mActivationTime = SDL_GetTicks() + 500;
    isTransitioning = false;
}

void GameManager::updateGameState()
{
    checkPlayerActivation();
    mGameWorld.updatePlayer();
    checkLevelTransitions();
}

void GameManager::checkPlayerActivation()
{
    if (SDL_GetTicks() > mActivationTime) 
    { 
        mPlayerActivated = true; 
    }
}

void GameManager::checkLevelTransitions()
{
    std::string levelToLoad;
    Vector spawnPosition;
    if (PhysicsEngine::HandlePlayerCollisions(
        mGameWorld.getPlayer(), 
        spawnPosition, 
        mGameWorld.getDoors(), 
        mGameWorld.getCrates(), 
        levelToLoad)
    ) 
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
    SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderSetScale(mRenderer, PLAYER_ZOOM_FACTOR, PLAYER_ZOOM_FACTOR);
    SDL_RenderClear(mRenderer);
}

void GameManager::finalizeRender() 
{ 
    SDL_RenderPresent(mRenderer); 
}

void GameManager::resetGame()
{
    mGameWorld.getPlayer().setVelocity(Vector(0, 0));
    mGameWorld.getCamera().setPosition(Vector(0, 0));
    std::cout << "Resetting game..." << std::endl;
}

bool GameManager::isRunning()
{ 
    return !mQuit; 
}

} // namespace BRTC