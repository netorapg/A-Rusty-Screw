#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SDL2/SDL.h>
#include "GameWorld.h"

namespace BRTC {
    class GameManager {
    public:
        GameManager(SDL_Window* window, SDL_Renderer* renderer);
        ~GameManager();
        
        void handleEvents();
        void update();
        void render();
        void resetGame();
        bool isRunning();

        GameWorld& getGameWorld() { return mGameWorld; }
        
    private:
        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        GameWorld mGameWorld;
        
        // Variáveis originais
        TTF_Font* mFont;
        TTF_Font* mSmallFont;
        Mix_Music* mMusic;
        Mix_Chunk* mJumpSound;
        
        bool isTransitioning = false;
        bool mQuit;
        bool increasing = true;
        bool mPlayerActivated;

        const float PLAYER_ZOOM_FACTOR = 3.5f;
        
        const Uint32 TRANSITION_DELAY = 500;
        const Uint32 HALF_TRANSITION = TRANSITION_DELAY / 2;
        Uint32 transitionStartTime = 0;
        Uint32 mActivationTime;
        
        int alpha = 0;
        std::string targetLevel;
        Vector targetSpawn;
        
        // Métodos originais
        void renderText(const char* text, int x, int y, TTF_Font* font);
        void handleTransition();
        void completeTransition(const Vector& currentVelocity);
        void updateGameState();
        void checkPlayerActivation();
        void checkLevelTransitions();
        void startTransition(const std::string& level, const Vector& spawn);
        void renderTransitionEffect();
        void prepareRender();
        void finalizeRender();
        void initializeRenderSettings();
        void initializeAudioSystem();
        void loadInitialLevel();
    };
}
#endif