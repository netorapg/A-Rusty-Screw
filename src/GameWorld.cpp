#include "../include/arscrew/GameWorld.h"
#include "../include/arscrew/PhysicsEngine.h"
#include "../include/arscrew/Globals.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace ARSCREW
{
    GameWorld::GameWorld(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mPlatformsTexture(nullptr)
        , mScrewsTexture(nullptr)
        , mapWidth(0)
        , mapHeight(0)
        , mPlayer(Vector(0, 0), renderer)
        , mCamera(SCREEN_WIDTH / 3.5f, SCREEN_HEIGHT / 3.5f)
    {
        loadTextures();
    }

    GameWorld::~GameWorld()
    {
        if (mPlatformsTexture) SDL_DestroyTexture(mPlatformsTexture);
        if (mScrewsTexture) SDL_DestroyTexture(mScrewsTexture);
    }

    void GameWorld::loadTextures()
    {
        mPlatformsTexturePath = "../assets/fulltile.png";
        mPlatformsTexture = IMG_LoadTexture(mRenderer, mPlatformsTexturePath.c_str());
        if (!mPlatformsTexture)
        {
            std::cerr << "Failed to load platforms texture: " << IMG_GetError() << std::endl;
        }
        
        mScrewsTexture = IMG_LoadTexture(mRenderer, "../assets/screw.png");
        if (!mScrewsTexture)
        {
            std::cerr << "Failed to load screws texture: " << IMG_GetError() << std::endl;
        }
    }

    void GameWorld::loadLevelFromTMX(const std::string& filePath)
    {
        std::cout << "Loading TMX file: " << filePath << std::endl;
        clearLevelData();
        
        XMLDocument doc;
        if (doc.LoadFile(filePath.c_str()))
        {
            std::cerr << "Failed to load TMX file: " << filePath << std::endl;
            return;
        }
        
        XMLElement* map = doc.FirstChildElement("map");
        int tileSize = map->IntAttribute("tilewidth");
        mapWidth = map->IntAttribute("width") * tileSize;
        mapHeight = map->IntAttribute("height") * tileSize;
        
        processMapLayers(map, tileSize);
        processObjectGroups(map, tileSize);
    }

    void GameWorld::clearLevelData()
    {
        mPlatforms.clear();
        mWalls.clear();
        mSolidPlatforms.clear();
        mRamps.clear();
        mCrates.clear();
        mDoors.clear();
        mDecorations.clear();
        mScrews.clear();
    }

    void GameWorld::updateWorld(float deltaTime)
    {
        // Atualizar caixotes
        for (auto& crate : mCrates)
        {
            crate.update(deltaTime);
            PhysicsEngine::HandleCollisions(crate, mWalls, mPlatforms, mSolidPlatforms, mRamps);
        }
    }

    void GameWorld::handleScrewCollisions()
    {
        if (!mPlayer.isAttacking()) return;

        SDL_Rect attackBox = mPlayer.getAttackHitbox();
        AttackType playerAttackType = mPlayer.getCurrentAttackType();

        for (auto it = mScrews.begin(); it != mScrews.end();)
        {
            if (it->isDestroyed())
            {
                ++it;
                continue;
            }

            SDL_Rect screwBox = it->getBoundingBox();
            bool overlap =
                (attackBox.x < screwBox.x + screwBox.w) &&
                (attackBox.x + attackBox.w > screwBox.x) &&
                (attackBox.y < screwBox.y + screwBox.h) &&
                (attackBox.y + attackBox.h > screwBox.y);

            if (overlap)
            {
                // Verifica se o tipo de ataque é compatível com o tipo de parafuso
                ScrewType screwType = it->getType();
                bool canDestroy = false;
                
                if (playerAttackType == AttackType::CUTTING && screwType == ScrewType::FLATHEAD)
                {
                    canDestroy = true;
                    std::cout << "Flathead screw destroyed with cutting attack!" << std::endl;
                }
                else if (playerAttackType == AttackType::PIERCING && screwType == ScrewType::PHILLIPS)
                {
                    canDestroy = true;
                    std::cout << "Phillips screw destroyed with piercing attack!" << std::endl;
                }
                else
                {
                    std::cout << "Wrong tool! Can't destroy this screw type." << std::endl;
                }
                
                if (canDestroy)
                {
                    if  (!mPlayer.isOnGround()) {
                        Vector velocity = mPlayer.getVelocity();
                        velocity.y = -300.0f;
                        mPlayer.setVelocity(velocity);
                        
                    }
                    it->destroy();
                    it = mScrews.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }
    }

    void GameWorld::renderWorld(SDL_Renderer* renderer, const Vector& cameraPos, const Vector& viewSize)
    {
        renderGameObjects(renderer, cameraPos, viewSize);
    }

    void GameWorld::renderGameObjects(SDL_Renderer* renderer, const Vector& cameraPos, const Vector& viewSize)
    {
        Vector snappedCameraPos(std::floor(cameraPos.x), std::floor(cameraPos.y));

        renderObjects(mDecorations, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mPlatforms, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mSolidPlatforms, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mWalls, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mCrates, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mDoors, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));
        renderObjects(mRamps, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));

        for (auto& screw : mScrews)
        {
            if (!screw.isDestroyed() && screw.isVisible(cameraPos, viewSize))
            {
                screw.render(renderer, snappedCameraPos);
            }
        }

        mPlayer.render(renderer, snappedCameraPos);
    }

    template<typename T>
    void GameWorld::renderObjects(std::list<T>& objects, SDL_Renderer* renderer, Vector& cameraPos, Vector& viewSize)
    {
        for (auto& obj : objects)
        {
            if (obj.isVisible(cameraPos, viewSize))
            {
                obj.render(renderer, cameraPos);
            }
        }
    }
    
    void GameWorld::processMapLayers(XMLElement* map, int tileSize)
    {
        std::unordered_map<int, int> tileTypeMap = 
        {
            {5, 2},{9, 2},{16, 2},{31, 2},{78, 2},  // Plataforma Sólida
            {18, 1}, // Plataforma vazada
            {64, 3},{15, 3},{41, 3}, {87, 3}, // Parede
            {65, 4}, // Caixote
            {89, 5} // Rampa
        };
        
        XMLElement* layer = map->FirstChildElement("layer");
        while (layer) {
            processLayers(layer, tileSize, tileTypeMap);
            layer = layer->NextSiblingElement("layer");
        }
    }

    void GameWorld::processLayers(XMLElement* layer, int tileSize, const std::unordered_map<int, int>& tileTypeMap)
    {
        const char* layerName = layer->Attribute("name");
        XMLElement* data = layer->FirstChildElement("data");
        
        if (!data || !(strcmp(layerName, "blocks") == 0 || strcmp(layerName, "decorations") == 0))
        {
            return;
        }
        
        std::string csvData = data->GetText();
        std::istringstream ss(csvData);
        std::string token;
        int index = 0;
        int layerWidth = layer->IntAttribute("width");
        
        while (std::getline(ss, token, ','))
        {
            int tileId = std::stoi(token);
            if (tileId == 0)
            {
                index++;
                continue;
            }
            
            mTilePosition.x = (index % layerWidth) * tileSize;
            mTilePosition.y = (index / layerWidth) * tileSize;
            
            if (strcmp(layerName, "blocks") == 0)
            {
                processBlockTile(tileId, mTilePosition, tileSize, tileTypeMap);
            }
            else if (strcmp(layerName, "decorations") == 0)
            {
                mDecorations.emplace_back(Vector(mTilePosition), Vector(tileSize, tileSize), mPlatformsTexture, tileId);
            }
            index++;
        }
    }

    void GameWorld::processBlockTile(int tileId, Vector& tilePosition, int tileSize, const std::unordered_map<int, int>& tileTypeMap)
    {
        auto it = tileTypeMap.find(tileId);
        if (it == tileTypeMap.end()) return;
        
        switch(it->second)
        {
            case 1: // Plataforma vazada
                mPlatforms.emplace_back(Vector(mTilePosition), Vector(tileSize, tileSize), mPlatformsTexture, tileId);
                break;
            case 2: // Plataforma sólida
                mSolidPlatforms.emplace_back(Vector(mTilePosition), Vector(tileSize, tileSize), mPlatformsTexture, tileId);
                break;
            case 3: // Parede
                mWalls.emplace_back(Vector(mTilePosition), Vector(tileSize, tileSize), mPlatformsTexture, tileId);
                break;
            case 4: // Caixote
                mCrates.emplace_back(Vector(mTilePosition), mRenderer);
                break;
            case 5: // Rampa
                mRamps.emplace_back(Vector(mTilePosition), Vector(tileSize, tileSize), mPlatformsTexture, tileId, RampType::BOTTOM_LEFT);
                break;
        }
    }

    void GameWorld::processObjectGroups(XMLElement* map, int tileSize)
    {
        XMLElement* objectGroup = map->FirstChildElement("objectgroup");
        while (objectGroup)
        {
            processObjectGroup(objectGroup, tileSize);
            objectGroup = objectGroup->NextSiblingElement("objectgroup");
        }
    }

    void GameWorld::processObjectGroup(XMLElement* objectGroup, int tileSize)
    {
        XMLElement* obj = objectGroup->FirstChildElement("object");
        while (obj)
        {
            processObject(obj, tileSize);
            obj = obj->NextSiblingElement("object");
        }
    }

    void GameWorld::processObject(XMLElement* obj, int tileSize)
    {
        const char* type = obj->Attribute("type");
        mAttributeSpawn.x = obj->FloatAttribute("x");
        mAttributeSpawn.y = obj->FloatAttribute("y");
        
        if (!type) return;
        
        if (strcmp(type, "player_spawn") == 0)
        {
            mPlayer.setPosition(Vector(mAttributeSpawn));
        }
        else if (strcmp(type, "door") == 0)
        {
            processDoorObject(obj, mAttributeSpawn, tileSize);
        }
        else if (strcmp(type, "screw_flathead") == 0)
        {
            mScrews.emplace_back(Vector(mAttributeSpawn), ScrewType::FLATHEAD, mScrewsTexture, mRenderer);
        }
        else if (strcmp(type, "screw_phillips") == 0)
        {
            mScrews.emplace_back(Vector(mAttributeSpawn), ScrewType::PHILLIPS, mScrewsTexture, mRenderer);
        }
    }

    void GameWorld::processDoorObject(XMLElement* obj, Vector& AttributeSpawn, int tileSize)
    {
        std::string target;
        XMLElement* properties = obj->FirstChildElement("properties");
        if (!properties) return;
        
        XMLElement* prop = properties->FirstChildElement("property");
        while (prop)
        {
            const char* name = prop->Attribute("name");
            if (!name) continue;
            
            if (strcmp(name, "target") == 0)
            {
                target = prop->Attribute("value");
            }
            else if (strcmp(name, "spawn_x") == 0)
            {
                mSpawnPosition.x = prop->FloatAttribute("value") * tileSize;
            }
            else if (strcmp(name, "spawn_y") == 0)
            {
                mSpawnPosition.y = prop->FloatAttribute("value") * tileSize;
            }
            prop = prop->NextSiblingElement("property");
        }
        
        mDoors.emplace_back(Vector(mAttributeSpawn), Vector(tileSize, tileSize), target, mRenderer, mPlatformsTexturePath, Vector(mSpawnPosition));
    }
}