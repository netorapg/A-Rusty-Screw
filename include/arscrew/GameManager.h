#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <array>

#include "GameWorld.h"
#include "HUD.h"
#include "Globals.h"
#include "GameOverScreen.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "GameState.h"

namespace ARSCREW
{
    class GameManager
    {
    public:
        GameManager(SDL_Window* window, SDL_Renderer* renderer);
        ~GameManager();
        
        void handleEvents();
        void update(float deltaTime);
        void render();
        void resetGame();
        bool isRunning();
        
    private:
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        TTF_Font* mFont;
        TTF_Font* mSmallFont;
        Mix_Music* mMusic;
        Mix_Chunk* mJumpSound;
        
        GameWorld mWorld;
        HUD mHUD;
        GameState mCurrentState;
        GameOverScreen mGameOverScreen;
        StartMenu mStartMenu;
        PauseMenu mPauseMenu;

        void updatePlaying(float deltaTime);
        void updateGameOver(float deltaTime);
        void updateMenu(float deltaTime);
        void updatePaused(float deltaTime);
        void renderPlaying();
        void renderGameOver();
        void renderMenu();
        void renderPaused();
        void switchToGameOver();
        void switchToPlaying();
        void switchToPaused();
        void switchToMenu();
        void restartGame();
        
        bool mQuit;
        bool mIsRunning;
        bool mPlayerActivated;
        Uint32 mActivationTime;
        
        // Sistema de transições
        bool isTransitioning;
        Uint32 transitionStartTime;
        std::string targetLevel;
        std::string mCurrentLevel;
        Vector targetSpawn;
        bool increasing;
        int alpha;
        
        // Configurações de renderização
        const float PLAYER_ZOOM_FACTOR = 3.0f;
        float effectiveScreenWidth;
        float effectiveScreenHeight;
        float mCameraSmoothSpeed = 5.0f;
        
        // Constantes de transição
        const Uint32 TRANSITION_DELAY = 500;
        const Uint32 HALF_TRANSITION = TRANSITION_DELAY / 2;
        
        // Sistema de parallax
        std::array<SDL_Texture*, 5> mParallaxLayers;
        std::array<float, 5> mParallaxFactors;
        
        // Métodos de inicialização
        void initializeRenderSettings();
        void initializeAudioSystem();
        void loadParallaxLayers();
        void centerCameraOnPlayer();
        
        // Métodos de atualização
        void handleTransition();
        void completeTransition(const Vector& currentVelocity);
        void updateGameState();
        void checkPlayerActivation();
        void updatePlayer();
        void checkLevelTransitions();
        void startTransition(const std::string& level, const Vector& spawn);
        void updateCamera();
        void updateCrates();
        
        // Métodos auxiliares de câmera
        Vector getPlayerCenter() const;
        Vector calculateCameraPosition(const Vector& playerCenter) const;
        void applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition);
        
        // Métodos de renderização
        void renderTransitionEffect();
        void prepareRender();
        void renderBackground();
        void renderParallaxLayer(int layerIndex, const Vector& cameraPos);
        void renderWorld();
        void renderHUD();
        void finalizeRender();
        void playJumpSound();
        
        // Métodos auxiliares
        std::string getCurrentLevelPath() const;
    };
}

#endif