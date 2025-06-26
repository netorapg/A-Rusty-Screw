#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <SDL2/SDL.h>
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
#include "Camera.h"
#include "Screw.h"
#include "Chicken.h"
#include "Enemy.h"
#include "Globals.h"

using namespace tinyxml2;

namespace ARSCREW
{
    class GameWorld
    {
    public:
        GameWorld(SDL_Renderer* renderer);
        ~GameWorld();
        
        void loadLevelFromTMX(const std::string& filePath);
        void clearLevelData();
        void updateWorld(float deltaTime);
        void renderWorld(SDL_Renderer* renderer, const Vector& cameraPos, const Vector& viewSize);
    
        // Getters para acesso às entidades
        const Player& getPlayer() const { return mPlayer; }
        const Camera& getCamera() const { return mCamera; }
        const Chicken& getChicken() const { return mChicken; }
        Player& getPlayer() { return mPlayer; }
        Camera& getCamera() { return mCamera; }
        Chicken& getChicken() { return mChicken; }
        std::list<Platform>& getPlatforms() { return mPlatforms; }
        std::list<SolidPlatform>& getSolidPlatforms() { return mSolidPlatforms; }
        std::list<Wall>& getWalls() { return mWalls; }
        std::list<Ramp>& getRamps() { return mRamps; }
        std::list<Crate>& getCrates() { return mCrates; }
        std::list<Door>& getDoors() { return mDoors; }
        std::list<Screw>& getScrews() { return mScrews; }
        std::list<Enemy>& getEnemies() { return mEnemies; }
        
        float getMapWidth() const { return mapWidth; }
        float getMapHeight() const { return mapHeight; }
        
        void handleScrewCollisions();
        void handleEnemyCollisions();
        
        // Controle do sistema de respawn dos parafusos
        void setScrewRespawnEnabled(bool enabled);
        bool isScrewRespawnEnabled() const { return mScrewRespawnEnabled; }
        void setScrewRespawnTime(float time);
        float getScrewRespawnTime() const { return mScrewRespawnTime; }

    private:
        SDL_Renderer* mRenderer;
        SDL_Texture* mPlatformsTexture;
        SDL_Texture* mScrewsTexture;
        std::string mPlatformsTexturePath;
        
        float mapWidth;
        float mapHeight;
        
        // Entidades do mundo
        std::list<Platform> mPlatforms;
        std::list<SolidPlatform> mSolidPlatforms;
        std::list<Ramp> mRamps;
        std::list<Wall> mWalls;
        std::list<Crate> mCrates;
        std::list<Door> mDoors;
        std::list<Decoration> mDecorations;
        std::list<Screw> mScrews;
        std::list<Enemy> mEnemies;
        
        Player mPlayer;
        Camera mCamera;
        Chicken mChicken;
        
        // Variáveis auxiliares
        Vector mTilePosition;
        Vector mAttributeSpawn;
        Vector mSpawnPosition;
        
        // Sistema de respawn dos parafusos
        bool mScrewRespawnEnabled;
        float mScrewRespawnTime;
        
        // Métodos de carregamento TMX
        void processMapLayers(XMLElement* map, int tileSize);
        void processObjectGroups(XMLElement* map, int tileSize);
        void processObjectGroup(XMLElement* objectGroup, int tileSize);
        void processObject(XMLElement* obj, int tileSize);
        void processLayers(XMLElement* layer, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
        void processBlockTile(int tileId, Vector& tilePosition, int tileSize, const std::unordered_map<int, int>& tileTypeMap);
        void processDoorObject(XMLElement* obj, Vector& AttributeSpawn, int tileSize);
        
        // Métodos de renderização
        void renderGameObjects(SDL_Renderer* renderer, const Vector& cameraPos, const Vector& viewSize);
        template<typename T>
        void renderObjects(std::list<T>& objects, SDL_Renderer* renderer, Vector& cameraPos, Vector& viewSize);
        
        void loadTextures();
    };
}

#endif