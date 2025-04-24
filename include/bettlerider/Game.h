#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <list>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <array>
#include <tinyxml2.h>

#include "Platform.h"
#include "SolidPlatform.h"
#include "Ramp.h"
#include "Wall.h"
#include "Crate.h"
#include "Door.h"
#include "Decoration.h"
#include "Player.h"
#include "PhysicsEngine.h"
#include "Camera.h"
#include "Globals.h"

using namespace tinyxml2;

namespace BRTC
{
    class Game
    {
        public:
            Game(SDL_Window* window, SDL_Renderer* renderer);
            ~Game();
            void handleEvents();
            void update();
            void render();
            void resetGame();
            bool isRunning();
        private:    
            SDL_Window* mWindow;
            SDL_Renderer* mRenderer;
            SDL_Texture* mBackgroundTexture;
            SDL_Texture* mPlatformsTexture;
            TTF_Font* mFont;
            TTF_Font* mSmallFont;
            Mix_Music* mMusic;
            Mix_Chunk* mJumpSound;

            bool isTransitioning = false;
            bool mQuit;
            bool increasing = true;
            bool mPlayerActivated;

            const float PLAYER_ZOOM_FACTOR = 2.5f;
            float effectiveScreenWidth = SCREEN_WIDTH / PLAYER_ZOOM_FACTOR;
            float effectiveScreenHeight = SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR;
            float mapWidth;
            float mapHeight;
            float mCameraSmoothSpeed = 5.0f;

            const Uint32 TRANSITION_DELAY = 500;
            const Uint32 HALF_TRANSITION = TRANSITION_DELAY / 2;
            Uint32 transitionStartTime = 0;
            Uint32 mActivationTime;

            int alpha = 0;

            std::list<Platform> mPlatforms;
            std::list<SolidPlatform> mSolidPlatforms;
            std::list<Ramp> mRamps;
            std::list<Wall> mWalls;
            std::list<Crate> mCrates;
            std::list<Door> mDoors;
            std::list<Decoration> mDecorations;
            std::string mPlatformsTexturePath;
            std::array<SDL_Texture*, 5> mParallaxLayers;
            std::array<float, 5> mParallaxFactors;
            std::string targetLevel;

            Player mPlayer;
            Camera mCamera;
            Vector mCAmeraPosition;
            Vector mCameraSize;
            Vector targetSpawn;
            Vector getPlayerCenter() const;
            Vector calculateCameraPosition(const Vector& playerCenter) const;
            Vector mTilePosition;
            Vector mAttributeSpawn;
            Vector mSpawnPosition;
            Vector mCameraTargetPosition;
            
            void renderText(const char* text, int x, int y, TTF_Font* font);
            void loadGameLevelFromTMX(const std::string& filePath);
            void clearLevelData();
            void processMapLayers(tinyxml2::XMLElement* map, int tileSize);
            void processObjectGroups(tinyxml2::XMLElement* map, int tileSize);
            void processObjectGroup(tinyxml2::XMLElement* objectGroup, int tileSize);
            void processObject(tinyxml2::XMLElement* obj, int tileSize);
            void processLayers(tinyxml2::XMLElement* layer, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
            void processBlockTile(int tileId, Vector& tilePosition, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
            void processDoorObject(tinyxml2::XMLElement* obj, Vector& AttributeSpawn, int tileSize);
            void handleTransition();
            void completeTransition(const Vector& currentVelocity);
            void updateGameState();
            void checkPlayerActivation();
            void updatePlayer();
            void checkLevelTransitions();
            void startTransition(const std::string& level, const Vector& spawn);
            void updateCamera();
            void applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition);
            void updateCrates();
            void renderTransitionEffect();
            void prepareRender();
            void renderBackground();
            void renderParallaxLayers(int layerIndex, const Vector& cameraPos);
            void renderGameObjects();
            template<typename T>
            void renderObjects(std::list<T>& objects, Vector& cameraPos, Vector& viewSize);
            void finalizeRender();
            void renderParallaxLayer(int layerIndex, const Vector& cameraPos);
            void initializeRenderSettings();
            void loadTextures();
            void initializeAudioSystem();
            void loadParallaxLayers();
            void loadInitialLevel();
            void centerCameraOnPlayer();

    };
}
#endif
