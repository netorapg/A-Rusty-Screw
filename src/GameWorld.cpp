#include "../include/bettlerider/GameWorld.h"

namespace BRTC {

GameWorld::GameWorld(SDL_Renderer* renderer) : 
    mRenderer(renderer),
    mPlayer(Vector(0, 0), renderer),
    mCamera(SCREEN_WIDTH / PLAYER_ZOOM_FACTOR, SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR),
    mPlayerActivated(true),
    mActivationTime(0),
    mPlatformsTexture(nullptr),
    isTransitioning(false),
    increasing(true),
    alpha(0)
{
    initializeRenderSettings();
    loadTextures();
    loadParallaxLayers();
    loadInitialLevel();
    mPlayerActivated = false;
    mActivationTime = SDL_GetTicks() + 500;
    centerCameraOnPlayer();
}

void GameWorld::initializeRenderSettings()
{
    SDL_RenderSetLogicalSize(mRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(mRenderer, SDL_TRUE);
}

void GameWorld::loadTextures()
{
    mPlatformsTexturePath = "../assets/fulltile.png";
    mPlatformsTexture = IMG_LoadTexture(mRenderer, mPlatformsTexturePath.c_str());
    if (!mPlatformsTexture)
    {
        std::cerr << "Failed to load platforms texture: " << IMG_GetError() << std::endl;
    }
}

void GameWorld::loadParallaxLayers() 
{
    const std::array<const char*, 5> layerFiles = 
    {
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

void GameWorld::loadInitialLevel() 
{
    loadGameLevelFromTMX("../map/level1.tmx");
}

void GameWorld::centerCameraOnPlayer() 
{
    Vector playerPos = mPlayer.getPosition();
    mCamera.setPosition(
        Vector(
            playerPos.x - (SCREEN_WIDTH / (2 * PLAYER_ZOOM_FACTOR)),
            playerPos.y - (SCREEN_HEIGHT / (2 * PLAYER_ZOOM_FACTOR))
        )
    );
}

GameWorld::~GameWorld()
{
    for (int i = 0; i < 5; i++) 
    {
        if (mParallaxLayers[i]) { SDL_DestroyTexture(mParallaxLayers[i]); }
    }
    if (mPlatformsTexture) { SDL_DestroyTexture(mPlatformsTexture); }
}

void GameWorld::loadGameLevelFromTMX(const std::string &filePath)
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
            mDecorations.emplace_back(
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
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
            mPlatforms.emplace_back(
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
        case 2: // Plataforma sólida
            mSolidPlatforms.emplace_back(
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
        case 3: // Parede
            mWalls.emplace_back(
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId
            );
            break;
        case 4: // Caixote
            mCrates.emplace_back(
                Vector(mTilePosition),  
                mRenderer
            );
            break;
        case 5: // Rampa
            mRamps.emplace_back(
                Vector(mTilePosition), 
                Vector(tileSize, tileSize), 
                mPlatformsTexture, 
                tileId,
                RampType::BOTTOM_LEFT
            );
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
    if (strcmp(type, "player_spawn") == 0) { 
        mPlayer.setPosition(Vector(mAttributeSpawn)); 
    } 
    else if(strcmp(type, "door") == 0) { 
        processDoorObject(obj, mAttributeSpawn, tileSize); 
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
        if (strcmp(name, "target") == 0) { 
            target = prop->Attribute("value"); 
        } 
        else if (strcmp(name, "spawn_x") == 0) { 
            mSpawnPosition.x = prop->FloatAttribute("value") * tileSize; 
        } 
        else if (strcmp(name, "spawn_y") == 0) { 
            mSpawnPosition.y = prop->FloatAttribute("value") * tileSize; 
        }
        prop = prop->NextSiblingElement("property");
    }
    mDoors.emplace_back(
        Vector(mAttributeSpawn), 
        Vector(tileSize, tileSize), 
        target, 
        mRenderer, 
        mPlatformsTexturePath, 
        Vector(mSpawnPosition)
    );
}

void GameWorld::updatePlayer() 
{
    if (mPlayerActivated) 
    {
        mPlayer.update(deltaTime);
        PhysicsEngine::HandleCollisions(mPlayer, mWalls, mPlatforms, mSolidPlatforms, mRamps);
    }
}

void GameWorld::updateCrates() 
{
    for (auto &crate : mCrates) 
    {
        crate.update(deltaTime);
        PhysicsEngine::HandleCollisions(crate, mWalls, mPlatforms, mSolidPlatforms, mRamps);
    }
}

void GameWorld::updateCamera() 
{
    Vector playerCenter = getPlayerCenter();
    Vector desiredPosition = calculateCameraPosition(playerCenter);

    desiredPosition.x = std::max(0.0f, std::min(desiredPosition.x, static_cast<float>(mapWidth) - effectiveScreenWidth));
    desiredPosition.y = std::max(0.0f, std::min(desiredPosition.y, static_cast<float>(mapHeight) - effectiveScreenHeight));
    
    applyCameraMargins(playerCenter, desiredPosition);

    Vector currentPos = mCamera.getPosition();
    Vector newPosition = Vector(
        currentPos.x + (desiredPosition.x - currentPos.x) * mCameraSmoothSpeed * deltaTime, 
        currentPos.y + (desiredPosition.y - currentPos.y) * mCameraSmoothSpeed * deltaTime
    );
    mCamera.setPosition(newPosition);
}

Vector GameWorld::getPlayerCenter() const 
{
    Vector playerPosition = mPlayer.getPosition();
    Vector playerSize = mPlayer.getSize();
    return Vector(
        playerPosition.x + playerSize.x / 2, 
        playerPosition.y + playerSize.y / 2
    );
}

Vector GameWorld::calculateCameraPosition(const Vector& playerCenter) const 
{
    return Vector(
        playerCenter.x - effectiveScreenWidth / 2,
        playerCenter.y - effectiveScreenHeight / 2
    );
}

void GameWorld::applyCameraMargins(const Vector& playerCenter, Vector& cameraPosition) 
{
    const float cameraMarginX = effectiveScreenWidth * 0.50f;
    const float cameraMarginY = effectiveScreenHeight * 0.50f;
    if (playerCenter.x < cameraPosition.x + cameraMarginX) 
    {
        cameraPosition.x = playerCenter.x - cameraMarginX;
    } 
    else if (playerCenter.x > cameraPosition.x + SCREEN_WIDTH - cameraMarginX) 
    {
        cameraPosition.x = playerCenter.x - (SCREEN_WIDTH - cameraMarginX);
    }
    if (playerCenter.y < cameraPosition.y + cameraMarginY) 
    {
        cameraPosition.y = playerCenter.y - cameraMarginY;
    }
    else if (playerCenter.y > cameraPosition.y + SCREEN_HEIGHT - cameraMarginY) 
    {
        cameraPosition.y = playerCenter.y - (SCREEN_HEIGHT - cameraMarginY);
    }
}

void GameWorld::renderBackground()
{
    Vector cameraPos = mCamera.getPosition();
    for (int i = 0; i < 5; i++) 
    {
        if (mParallaxLayers[i]) 
        {
            renderParallaxLayer(i, cameraPos);
        }
    }
}

void GameWorld::renderParallaxLayer(int layerIndex, const Vector& cameraPos) 
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

void GameWorld::renderGameObjects() 
{
    Vector cameraPos = mCamera.getPosition();
    Vector snappedCameraPos(std::floor(cameraPos.x), std::floor(cameraPos.y));
    Vector viewSize(effectiveScreenWidth, effectiveScreenHeight);

    renderObjects(mDecorations, snappedCameraPos, viewSize);
    renderObjects(mPlatforms, snappedCameraPos, viewSize);
    renderObjects(mSolidPlatforms, snappedCameraPos, viewSize);
    renderObjects(mWalls, snappedCameraPos, viewSize);
    renderObjects(mCrates, snappedCameraPos, viewSize);
    renderObjects(mDoors, snappedCameraPos, viewSize); 
    renderObjects(mRamps, snappedCameraPos, viewSize);
    mPlayer.render(mRenderer, snappedCameraPos);
}

template<typename T>
void GameWorld::renderObjects(std::list<T>& objects, Vector& cameraPos, Vector& viewSize) 
{
    for (auto& obj : objects) 
    {
        if (obj.isVisible(cameraPos, viewSize)) 
        { 
            obj.render(mRenderer, cameraPos); 
        }
    }
}

} // namespace BRTC