#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <list>
#include <string>
#include <unordered_map>
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

namespace BRTC {
    class GameWorld {
    public:
        GameWorld(SDL_Renderer* renderer);
        ~GameWorld();
        
        // Métodos originais transferidos do Game
        void loadGameLevelFromTMX(const std::string& filePath);
        void clearLevelData();
        void updatePlayer();
        void updateCrates();
        void updateCamera();
        Vector getPlayerCenter() const;
        Vector calculateCameraPosition(const Vector& playerCenter) const;
        void applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition);
        void renderBackground();
        void renderParallaxLayers(int layerIndex, const Vector& cameraPos);
        void renderGameObjects();
        template<typename T>
        void renderObjects(std::list<T>& objects, Vector& cameraPos, Vector& viewSize);
        void renderParallaxLayer(int layerIndex, const Vector& cameraPos);
        void centerCameraOnPlayer();
        void loadTextures();
        void loadParallaxLayers();
        
        // Variáveis originais
        SDL_Texture* mBackgroundTexture;
        SDL_Texture* mPlatformsTexture;
        std::array<SDL_Texture*, 5> mParallaxLayers;
        std::array<float, 5> mParallaxFactors;
        std::string mPlatformsTexturePath;
        
        float mapWidth;
        float mapHeight;
        const float PLAYER_ZOOM_FACTOR = 3.5f;
        float effectiveScreenWidth = SCREEN_WIDTH / PLAYER_ZOOM_FACTOR;
        float effectiveScreenHeight = SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR;
        float mCameraSmoothSpeed = 5.0f;

        Uint32 mActivationTime;
        
        std::list<Platform> mPlatforms;
        std::list<SolidPlatform> mSolidPlatforms;
        std::list<Ramp> mRamps;
        std::list<Wall> mWalls;
        std::list<Crate> mCrates;
        std::list<Door> mDoors;
        std::list<Decoration> mDecorations;
        
        Player mPlayer;
        Camera mCamera;
        Vector mCAmeraPosition;
        Vector mCameraSize;
        Vector mTilePosition;
        Vector mAttributeSpawn;
        Vector mSpawnPosition;
        Vector mCameraTargetPosition;

        Player& getPlayer() { return mPlayer; }
        std::list<Crate>& getCrates() { return mCrates; }
        std::list<Door>& getDoors() { return mDoors; }
        std::list<Platform>& getPlatforms() { return mPlatforms; }
        std::list<SolidPlatform>& getSolidPlatforms() { return mSolidPlatforms; }
        std::list<Ramp>& getRamps() { return mRamps; }
        std::list<Wall>& getWalls() { return mWalls; }
        std::list<Decoration>& getDecorations() { return mDecorations; }
        Camera& getCamera() { return mCamera; }

        void setDeltaTime(float dt) { deltaTime = dt; }
        float getDeltaTime() const { return deltaTime; }

        
        
    private:
        SDL_Renderer* mRenderer;

        bool mPlayerActivated;
        bool isTransitioning = false;
        bool increasing = true;

        int alpha = 0;

        float deltaTime;
        
        void processMapLayers(XMLElement* map, int tileSize);
        void processObjectGroups(XMLElement* map, int tileSize);
        void processObjectGroup(XMLElement* objectGroup, int tileSize);
        void processObject(XMLElement* obj, int tileSize);
        void processLayers(XMLElement* layer, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
        void processBlockTile(int tileId, Vector& tilePosition, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
        void processDoorObject(XMLElement* obj, Vector& AttributeSpawn, int tileSize);
        void loadInitialLevel();
        void initializeRenderSettings();
    };
}
#endif