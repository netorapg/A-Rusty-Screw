#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <array>

#include "GameWorld.h"
#include "Globals.h"

namespace ARSCREW
{
    class GameManager
    {
    public:
        GameManager(SDL_Window* window, SDL_Renderer* renderer);
        ~GameManager();
        
        void handleEvents();
        void update();
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
        
        bool mQuit;
        bool mPlayerActivated;
        Uint32 mActivationTime;
        
        // Sistema de transições
        bool isTransitioning;
        Uint32 transitionStartTime;
        std::string targetLevel;
        Vector targetSpawn;
        bool increasing;
        int alpha;
        
        // Configurações de renderização
        const float PLAYER_ZOOM_FACTOR = 3.5f;
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
        void finalizeRender();
    };
}

#endif