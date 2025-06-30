#include "../include/arscrew/GameWorld.h"
#include "../include/arscrew/CollisionEngine.h"
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
        , mChicken(Vector(0, 0), renderer)
        , mScrewRespawnEnabled(true)
        , mScrewRespawnTime(10.0f)
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
        mSolidPlatforms.clear();
        mRamps.clear();
        mCrates.clear();
        mDoors.clear();
        mDecorations.clear();
        mScrews.clear();
        mEnemies.clear();
        mPunktauro.reset(); // Limpar o boss Punktauro
    }

    void GameWorld::updateWorld(float deltaTime)
    {
        handleInput();
        // Atualizar caixotes
        for (auto& crate : mCrates)
        {
            crate.update(deltaTime);
            CollisionEngine::HandleCollisions(crate, mPlatforms, mSolidPlatforms, mRamps);
        }
        
        mChicken.update(deltaTime);
        mChicken.followPlayer(mPlayer, deltaTime);
        CollisionEngine::HandleCollisions(mChicken, mPlatforms, mSolidPlatforms, mRamps);
        // Atualizar parafusos (para o sistema de respawn)
        for (auto& screw : mScrews)
        {
            screw.update(deltaTime);
        }
        // Atualizar inimigos
        for (auto& enemy : mEnemies)
        {
            if (!enemy.isDestroyed())
            {
                enemy.updateWithPlayer(mPlayer, deltaTime);
                CollisionEngine::HandleCollisions(enemy, mPlatforms, mSolidPlatforms, mRamps);
            }
        }
        
        // Atualizar boss Punktauro
        if (mPunktauro && !mPunktauro->isDead())
        {
            mPunktauro->updateWithPlayer(mPlayer, deltaTime);
            CollisionEngine::HandleCollisions(*mPunktauro, mPlatforms, mSolidPlatforms, mRamps);
        }
        
        // Opcional: Remover inimigos destruídos após um tempo
        mEnemies.remove_if([](const Enemy& enemy) { 
            return enemy.isDestroyed(); 
        });
    }

    void GameWorld::handleInput()
    {
        mInputManager.update();
        
        // Movimento contínuo
        bool movingLeft = mInputManager.isActionPressed(InputAction::MOVE_LEFT);
        bool movingRight = mInputManager.isActionPressed(InputAction::MOVE_RIGHT);
        mPlayer.setMovementInput(movingLeft, movingRight);
        
        // Ações que precisam ser "just pressed" (não repetir)
        if (mInputManager.isActionJustPressed(InputAction::JUMP)) {
            mPlayer.jump();
        }
        
        if (mInputManager.isActionJustPressed(InputAction::ATTACK)) {
            mPlayer.startAttack();
        }
        
        if (mInputManager.isActionJustPressed(InputAction::DASH)) {
            mPlayer.startDash();
        }
        
        if (mInputManager.isActionJustPressed(InputAction::SWITCH_ATTACK)) {
            mPlayer.switchAttackType();
        }
        
        // Agachar/passar pela plataforma (contínuo)
        if (mInputManager.isActionPressed(InputAction::CROUCH)) {
            mPlayer.passThroughPlatform(true);
        } else {
            mPlayer.passThroughPlatform(false);
        }
        
        // Debug toggle (just pressed)
        if (mInputManager.isActionJustPressed(InputAction::DEBUG_TOGGLE)) {
            mPlayer.toggleDebugDisplay();
            for (auto& enemy : mEnemies) {
                enemy.toggleDebugDisplay();
            }
            if (mPunktauro) {
                mPunktauro->toggleDebugDisplay();
            }
        }
    }

    void GameWorld::handleScrewCollisions()
    {
        if (!mPlayer.isAttacking()) return;

        SDL_Rect attackBox = mPlayer.getAttackHitbox();
        AttackType playerAttackType = mPlayer.getCurrentAttackType();

        for (auto it = mScrews.begin(); it != mScrews.end(); ++it)
        {
            if (it->isDestroyed()) continue;

            SDL_Rect screwBox = it->getBoundingBox();
            bool overlap =
                (attackBox.x < screwBox.x + screwBox.w) &&
                (attackBox.x + attackBox.w > screwBox.x) &&
                (attackBox.y < screwBox.y + screwBox.h) &&
                (attackBox.y + attackBox.h > screwBox.y);

            if (overlap)
            {
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
                    if (!mPlayer.isOnGround()) {
                        Vector velocity = mPlayer.getVelocity();
                        velocity.y = -300.0f;
                        mPlayer.setVelocity(velocity);
                        std::cout << "Air screw hit! Player boosted upward!" << std::endl;
                    }
                    
                    it->destroy(); // Agora apenas marca como destruído, não remove da lista
                }
            }
        }
    }

   void GameWorld::handleEnemyCollisions()
{
    for (auto& enemy : mEnemies)
    {
        if (enemy.isDestroyed()) continue;

        SDL_Rect enemyBox = enemy.getBoundingBox();
        
        // Verificar se player está atacando
        if (mPlayer.isAttacking())
        {
            SDL_Rect playerAttackBox = mPlayer.getAttackHitbox();
            
            bool attackOverlap =
                (playerAttackBox.x < enemyBox.x + enemyBox.w) &&
                (playerAttackBox.x + playerAttackBox.w > enemyBox.x) &&
                (playerAttackBox.y < enemyBox.y + enemyBox.h) &&
                (playerAttackBox.y + playerAttackBox.h > enemyBox.y);

            if (attackOverlap)
            {
                std::cout << "Enemy hit by player attack!" << std::endl;
                enemy.takeDamage(25); // Player causa 25 de dano
                
                // Knockback no inimigo
                Vector enemyVelocity = enemy.getVelocity();
                int playerDirection = mPlayer.getFacingDirection();
                enemyVelocity.x = playerDirection * 200.0f; // Empurrar na direção do ataque
                enemyVelocity.y = -100.0f; // Pequeno pulo para cima
                enemy.setVelocity(enemyVelocity);
                
                continue; // Pular verificação de dano ao player se já atacou
            }
        }
        
        // Verificar colisão com hurtbox do player (para receber dano)
        // Só causa dano se o player não estiver invulnerável
        if (!mPlayer.isInvulnerable())
        {
            SDL_Rect playerBox = mPlayer.getHurtbox();
            
            bool hurtOverlap =
                (playerBox.x < enemyBox.x + enemyBox.w) &&
                (playerBox.x + playerBox.w > enemyBox.x) &&
                (playerBox.y < enemyBox.y + enemyBox.h) &&
                (playerBox.y + playerBox.h > enemyBox.y);

            if (hurtOverlap)
            {
                std::cout << "Player hit by enemy!" << std::endl;
                mPlayer.takeDamage(enemy.getDamage());
            }
        }
    }
}

    void GameWorld::handlePunktauroCollisions()
    {
        if (!mPunktauro || mPunktauro->isDead()) return;

        // Usar a hurtbox da cabeça em vez da hurtbox do corpo completo
        SDL_Rect bossHeadHurtbox = mPunktauro->getHeadHurtbox();
        
        // Verificar se player está atacando o boss (apenas na cabeça)
        if (mPlayer.isAttacking() && !mPunktauro->isInvulnerable())
        {
            SDL_Rect playerAttackBox = mPlayer.getAttackHitbox();
            
            // Debug temporário - remover depois
            std::cout << "DEBUG - Player attacking!" << std::endl;
            std::cout << "  Player attack box: x=" << playerAttackBox.x << " y=" << playerAttackBox.y 
                      << " w=" << playerAttackBox.w << " h=" << playerAttackBox.h << std::endl;
            std::cout << "  Boss head hurtbox: x=" << bossHeadHurtbox.x << " y=" << bossHeadHurtbox.y 
                      << " w=" << bossHeadHurtbox.w << " h=" << bossHeadHurtbox.h << std::endl;
            
            bool attackOverlap =
                (playerAttackBox.x < bossHeadHurtbox.x + bossHeadHurtbox.w) &&
                (playerAttackBox.x + playerAttackBox.w > bossHeadHurtbox.x) &&
                (playerAttackBox.y < bossHeadHurtbox.y + bossHeadHurtbox.h) &&
                (playerAttackBox.y + playerAttackBox.h > bossHeadHurtbox.y);

            std::cout << "  Overlap result: " << (attackOverlap ? "YES" : "NO") << std::endl;

            if (attackOverlap)
            {
                int damage = 25; // Dano base do player
                
                // Dano extra baseado no tipo de ataque
                if (mPlayer.getCurrentAttackType() == AttackType::PIERCING) {
                    damage += 10; // Ataque perfurante causa mais dano no boss
                    std::cout << "CRITICAL HIT! Piercing attack against Punktauro's head!" << std::endl;
                } else {
                    std::cout << "HEADSHOT! Attack hit Punktauro's head!" << std::endl;
                }
                
                mPunktauro->takeDamage(damage);
                std::cout << "Punktauro hit in the head! Phase: " << static_cast<int>(mPunktauro->getCurrentPhase()) 
                          << " Health: " << mPunktauro->getCurrentHealth() << "/" << mPunktauro->getMaxHealth() << std::endl;
                
                // Impulso para o jogador (similar ao acertar parafuso no ar)
                if (!mPlayer.isOnGround()) {
                    Vector playerVelocity = mPlayer.getVelocity();
                    playerVelocity.y = -300.0f; // Mesmo impulso que o parafuso
                    mPlayer.setVelocity(playerVelocity);
                    std::cout << "Air headshot! Player boosted upward!" << std::endl;
                }
                
                // Knockback no boss (menor que inimigos normais por ser mais pesado)
                Vector bossVelocity = mPunktauro->getVelocity();
                int playerDirection = mPlayer.getFacingDirection();
                bossVelocity.x = playerDirection * 100.0f; // Knockback menor para o boss
                bossVelocity.y = -50.0f; // Pequeno pulo
                mPunktauro->setVelocity(bossVelocity);
                
                // Verificar se o boss foi derrotado
                if (mPunktauro->isDead())
                {
                    std::cout << "=== PUNKTAURO DERROTADO! PARABÉNS! ===" << std::endl;
                    // Aqui você pode adicionar efeitos especiais, som de vitória, recompensas, etc.
                }
                
                return; // Pular verificação de dano ao player se já atacou
            }
        }
        
        // Verificar se o boss está atacando o player
        if (mPunktauro->isAttacking() && !mPlayer.isInvulnerable())
        {
            SDL_Rect playerHurtbox = mPlayer.getHurtbox();
            SDL_Rect bossAttackBox = mPunktauro->getAttackHitbox();
            
            bool hurtOverlap =
                (playerHurtbox.x < bossAttackBox.x + bossAttackBox.w) &&
                (playerHurtbox.x + playerHurtbox.w > bossAttackBox.x) &&
                (playerHurtbox.y < bossAttackBox.y + bossAttackBox.h) &&
                (playerHurtbox.y + playerHurtbox.h > bossAttackBox.y);

            if (hurtOverlap)
            {
                int bossDamage = mPunktauro->getDamage();
                mPlayer.takeDamage(bossDamage);
                
                std::cout << "Player hit by Punktauro! Damage: " << bossDamage 
                          << " (Phase " << static_cast<int>(mPunktauro->getCurrentPhase()) << ")" << std::endl;
                
                // Knockback no player
                Vector playerVelocity = mPlayer.getVelocity();
                int bossDirection = mPunktauro->getFacingDirection();
                playerVelocity.x = bossDirection * 800.0f; // Knockback forte do boss
                playerVelocity.y = -150.0f; // Pulo para cima
                mPlayer.setVelocity(playerVelocity);
            }
        }
    }

    void GameWorld::setScrewRespawnEnabled(bool enabled)
    {
        mScrewRespawnEnabled = enabled;
        for (auto& screw : mScrews)
        {
            screw.setRespawnEnabled(enabled);
        }
    }

    void GameWorld::setScrewRespawnTime(float time)
    {
        mScrewRespawnTime = time;
        for (auto& screw : mScrews)
        {
            screw.setRespawnTime(time);
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

        for (auto& enemy : mEnemies)
        {
            if (enemy.isVisible(cameraPos, viewSize))
            {
                enemy.render(renderer, snappedCameraPos);
            }
        }

        // Renderizar boss Punktauro (por último para ficar na frente)
        if (mPunktauro && !mPunktauro->isDead() && mPunktauro->isVisible(cameraPos, viewSize))
        {
            mPunktauro->render(renderer, snappedCameraPos);
        }

        mChicken.render(renderer, snappedCameraPos);
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
            {18, 1}, // Plataforma vazada
            {5, 2},{9, 2},{16, 2},{31, 2},{78, 2}, {23, 2},   // Plataforma Sólida
            {64, 2},{15, 2},{41, 2}, {87, 2}, {15, 2}, {13, 2},
            {65, 3}, // Caixote
            {89, 4} // Rampa
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
            case 3: // Caixote
                mCrates.emplace_back(Vector(mTilePosition), mRenderer);
                break;
            case 4: // Rampa
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
            mChicken.setPosition(Vector(mAttributeSpawn.x - 30, mAttributeSpawn.y));
        }
        else if (strcmp(type, "door") == 0)
        {
            processDoorObject(obj, mAttributeSpawn, tileSize);
        }
        else if (strcmp(type, "screw_flathead") == 0)
        {
            mScrews.emplace_back(Vector(mAttributeSpawn), ScrewType::FLATHEAD, mScrewsTexture, mRenderer);
            mScrews.back().setRespawnEnabled(mScrewRespawnEnabled);
            mScrews.back().setRespawnTime(mScrewRespawnTime);
        }
        else if (strcmp(type, "screw_phillips") == 0)
        {
            mScrews.emplace_back(Vector(mAttributeSpawn), ScrewType::PHILLIPS, mScrewsTexture, mRenderer);
            mScrews.back().setRespawnEnabled(mScrewRespawnEnabled);
            mScrews.back().setRespawnTime(mScrewRespawnTime);
        }
        else if (strcmp(type, "enemy_spawn") == 0)
        {
            mEnemies.emplace_back(Vector(mAttributeSpawn), mRenderer);
            std::cout << "Enemy spawned at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y << std::endl;
        }
        else if (strcmp(type, "boss_spawn") == 0 || strcmp(type, "punktauro_spawn") == 0)
        {
            mPunktauro = std::make_unique<Punktauro>(Vector(mAttributeSpawn), mRenderer);
            std::cout << "PUNKTAURO BOSS spawned at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y << std::endl;
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