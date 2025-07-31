#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <SDL2/SDL.h>
#include <list>
#include <string>
#include <unordered_map>
#include <array>
#include <memory>
#include <functional>
#include <tinyxml2.h>

#include "Platform.h"
#include "SolidPlatform.h"
#include "Crate.h"
#include "Door.h"
#include "Decoration.h"
#include "Gate.h"
#include "ToolTip.h"
#include "Player.h"
#include "Camera.h"
#include "Screw.h"
#include "Chicken.h"
#include "Enemy.h"
#include "Punktauro.h"
#include "Globals.h"
#include "InputManager.h"

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
        void handleInput();
    
        // Getters para acesso às entidades
        const Player& getPlayer() const { return mPlayer; }
        const Camera& getCamera() const { return mCamera; }
        const Chicken& getChicken() const { return mChicken; }
        Player& getPlayer() { return mPlayer; }
        Camera& getCamera() { return mCamera; }
        Chicken& getChicken() { return mChicken; }
        InputManager& getInputManager() { return mInputManager; }
        std::list<Platform>& getPlatforms() { return mPlatforms; }
        std::list<SolidPlatform>& getSolidPlatforms() { return mSolidPlatforms; }
        std::list<Crate>& getCrates() { return mCrates; }
        std::list<Door>& getDoors() { return mDoors; }
        std::list<Gate>& getGates() { return mGates; }
        std::list<ToolTip>& getToolTips() { return mToolTips; }
        std::list<Screw>& getScrews() { return mScrews; }
        std::list<Enemy>& getEnemies() { return mEnemies; }
        
        void setEnemyKilledCallback(const std::function<void()>& callback) { mEnemyKilledCallback = callback; }

        // Getter para o boss Punktauro
        Punktauro* getPunktauro() { return mPunktauro.get(); }
        bool hasPunktauro() const { return mPunktauro != nullptr; }
        
        float getMapWidth() const { return mapWidth; }
        float getMapHeight() const { return mapHeight; }
        
        void handleScrewCollisions();
        void handleEnemyCollisions();
        void handlePunktauroCollisions();
        void handleGateCollisions();
        void handleToolTipCollisions();
        
        // Métodos auxiliares para encontrar parafusos
        Screw* findScrewByPosition(const Vector& position, float tolerance = 10.0f);
        Screw* findScrewById(int id); // Para quando implementarmos IDs
        
        // Controle do sistema de respawn dos parafusos
        void setScrewRespawnEnabled(bool enabled);
        bool isScrewRespawnEnabled() const { return mScrewRespawnEnabled; }
        
        // Callbacks para sons
        void setAttackSoundCallback(std::function<void()> callback) { mAttackSoundCallback = callback; }
        void setTooltipSoundCallback(std::function<void()> callback) { mTooltipSoundCallback = callback; }
        void setPlayerHitSoundCallback(std::function<void()> callback) { mPlayerHitSoundCallback = callback; }
        void setEnemyHitSoundCallback(std::function<void()> callback) { mEnemyHitSoundCallback = callback; }
        void setEnemyDeathSoundCallback(std::function<void()> callback) { mEnemyDeathSoundCallback = callback; }
        void setPunktauroAccelerateSoundCallback(std::function<void()> callback) { mPunktauroAccelerateSoundCallback = callback; }
        void setPunktauroJumpSoundCallback(std::function<void()> callback) { mPunktauroJumpSoundCallback = callback; }
        void setPunktauroDeathSoundCallback(std::function<void()> callback) { mPunktauroDeathSoundCallback = callback; }
        void setGateSoundCallback(std::function<void()> callback) { mGateSoundCallback = callback; }
        void setScrewRespawnTime(float time);
        float getScrewRespawnTime() const { return mScrewRespawnTime; }

    private:
        SDL_Renderer* mRenderer;
        SDL_Texture* mPlatformsTexture;
        SDL_Texture* mScrewsTexture;
        SDL_Texture* mToolTipsTexture;
        std::string mPlatformsTexturePath;

        InputManager mInputManager;
        
        float mapWidth;
        float mapHeight;
        
        // Entidades do mundo
        std::list<Platform> mPlatforms;
        std::list<SolidPlatform> mSolidPlatforms;
        std::list<Crate> mCrates;
        std::list<Door> mDoors;
        std::list<Decoration> mDecorations;
        std::list<Gate> mGates;
        std::list<ToolTip> mToolTips;
        std::list<Screw> mScrews;
        std::list<Enemy> mEnemies;
        
        Player mPlayer;
        Camera mCamera;
        Chicken mChicken;
        
        // Boss Punktauro (único)
        std::unique_ptr<Punktauro> mPunktauro;
        
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
        void processGateObject(XMLElement* obj, Vector& AttributeSpawn, int tileSize);
        
        // Métodos de renderização
        void renderGameObjects(SDL_Renderer* renderer, const Vector& cameraPos, const Vector& viewSize);
        template<typename T>
        void renderObjects(std::list<T>& objects, SDL_Renderer* renderer, Vector& cameraPos, Vector& viewSize);
        
        void loadTextures();
        
        // Callbacks para sons
        std::function<void()> mAttackSoundCallback;
        std::function<void()> mTooltipSoundCallback;
        std::function<void()> mPlayerHitSoundCallback;
        std::function<void()> mEnemyHitSoundCallback;
        std::function<void()> mEnemyDeathSoundCallback;
        std::function<void()> mPunktauroAccelerateSoundCallback;
        std::function<void()> mPunktauroJumpSoundCallback;
        std::function<void()> mPunktauroDeathSoundCallback;
        std::function<void()> mGateSoundCallback;
        
        // Flags para controlar repetição de sons
        bool mPlayerHitSoundPlayed;
        bool mPunktauroDeathSoundPlayed;

        std::function<void()> mEnemyKilledCallback;

    };
}

#endif