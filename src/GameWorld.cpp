#include "../include/arscrew/GameWorld.h"
#include "../include/arscrew/CollisionEngine.h"
#include "../include/arscrew/Globals.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace ARSCREW
{
    GameWorld::GameWorld(SDL_Renderer* renderer)
        : mRenderer(renderer)
        , mPlatformsTexture(nullptr)
        , mScrewsTexture(nullptr)
        , mToolTipsTexture(nullptr)
        , mapWidth(0)
        , mapHeight(0)
        , mPlayer(Vector(0, 0), renderer)
        , mCamera(SCREEN_WIDTH / 3.5f, SCREEN_HEIGHT / 3.5f)
        , mChicken(Vector(0, 0), renderer)
        , mScrewRespawnEnabled(true)
        , mScrewRespawnTime(3.0f)
        , mPlayerHitSoundPlayed(false)
        , mPunktauroDeathSoundPlayed(false)
    {
        loadTextures();
    }

    GameWorld::~GameWorld()
    {
        if (mPlatformsTexture) SDL_DestroyTexture(mPlatformsTexture);
        if (mScrewsTexture) SDL_DestroyTexture(mScrewsTexture);
        if (mToolTipsTexture) SDL_DestroyTexture(mToolTipsTexture);
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
        
        mToolTipsTexture = IMG_LoadTexture(mRenderer, "../assets/phillips.png");
        if (!mToolTipsTexture)
        {
            std::cerr << "Failed to load tooltips texture: " << IMG_GetError() << std::endl;
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
        mCrates.clear();
        mDoors.clear();
        mDecorations.clear();
        mGates.clear();
        mToolTips.clear();
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
            CollisionEngine::HandleCollisions(crate, mPlatforms, mSolidPlatforms);
        }
        
        mChicken.update(deltaTime);
        mChicken.followPlayer(mPlayer, deltaTime);
        CollisionEngine::HandleCollisions(mChicken, mPlatforms, mSolidPlatforms);
        // Atualizar parafusos (para o sistema de respawn)
        for (auto& screw : mScrews)
        {
            screw.update(deltaTime);
        }
        
        // Atualizar portões
        for (auto& gate : mGates)
        {
            gate.update(deltaTime);
        }
        
        // Atualizar pontas (tool tips)
        for (auto& toolTip : mToolTips)
        {
            toolTip.update(deltaTime);
        }
        // Atualizar inimigos
        for (auto& enemy : mEnemies)
        {
            if (!enemy.isDestroyed())
            {
                enemy.updateWithPlayer(mPlayer, deltaTime);
                CollisionEngine::HandleCollisions(enemy, mPlatforms, mSolidPlatforms);
            }
        }
        
        // Atualizar boss Punktauro
        if (mPunktauro && !mPunktauro->isDead())
        {
            mPunktauro->updateWithPlayer(mPlayer, deltaTime);
            CollisionEngine::HandleCollisions(*mPunktauro, mPlatforms, mSolidPlatforms);
        }
        
        // Lidar com colisões
        handleGateCollisions();
        handleToolTipCollisions();
        
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
            // Tocar som de ataque
            if (mAttackSoundCallback) {
                mAttackSoundCallback();
            }
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

            if (attackOverlap && !enemy.isInvulnerable())
            {
                int damage = mPlayer.getAttackDamage(); // Usar dano diferenciado baseado no tipo de ataque
                std::cout << "Enemy hit by player attack! Damage: " << damage << " (Type: " 
                          << (mPlayer.getCurrentAttackType() == AttackType::CUTTING ? "CUTTING" : "PIERCING") << ")" << std::endl;
                
                bool wasAlive = !enemy.isDead();
                enemy.takeDamage(damage);
                
                // Tocar som apropriado apenas se o dano foi aplicado e som ainda não foi tocado
                if (enemy.isDead() && wasAlive && !enemy.hasDeathSoundPlayed()) {
                    // Inimigo morreu
                    if (mEnemyDeathSoundCallback) {
                        mEnemyDeathSoundCallback();
                        enemy.setDeathSoundPlayed(true);
                    }
                } else if (wasAlive && !enemy.hasHitSoundPlayed()) {
                    // Inimigo atingido mas ainda vivo
                    if (mEnemyHitSoundCallback) {
                        mEnemyHitSoundCallback();
                        enemy.setHitSoundPlayed(true);
                    }
                }
                
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
        bool playerHitThisFrame = false;
        if (!mPlayer.isInvulnerable())
        {
            SDL_Rect playerBox = mPlayer.getHurtbox();
            
            bool hurtOverlap =
                (playerBox.x < enemyBox.x + enemyBox.w) &&
                (playerBox.x + playerBox.w > enemyBox.x) &&
                (playerBox.y < enemyBox.y + enemyBox.h) &&
                (playerBox.y + playerBox.h > enemyBox.y);

            if (hurtOverlap && !mPlayerHitSoundPlayed)
            {
                std::cout << "Player hit by enemy!" << std::endl;
                int oldHealth = mPlayer.getCurrentHealth();
                mPlayer.takeDamage(enemy.getDamage());
                
                // Tocar som apenas se o dano foi realmente aplicado
                if (mPlayer.getCurrentHealth() < oldHealth && mPlayerHitSoundCallback) {
                    mPlayerHitSoundCallback();
                    mPlayerHitSoundPlayed = true;
                }
                playerHitThisFrame = true;
            }
        }
        
        // Reset da flag quando não há mais colisão
        if (!playerHitThisFrame && mPlayer.isInvulnerable()) {
            mPlayerHitSoundPlayed = false;
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

            if (attackOverlap && !mPunktauro->isInvulnerable())
            {
                int damage = mPlayer.getAttackDamage(); // Usar dano diferenciado baseado no tipo de ataque
                
                // Mensagem de feedback baseada no tipo de ataque
                if (mPlayer.getCurrentAttackType() == AttackType::PIERCING) {
                    std::cout << "CRITICAL HIT! Piercing attack against Punktauro's head! Damage: " << damage << std::endl;
                } else {
                    std::cout << "HEADSHOT! Cutting attack hit Punktauro's head! Damage: " << damage << std::endl;
                }
                
                mPunktauro->takeDamage(damage);
                
                // O som de morte será tocado automaticamente pelo próprio Punktauro
                
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
                
                // Tocar som de jogador atingido
                if (mPlayerHitSoundCallback) {
                    mPlayerHitSoundCallback();
                }
                
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
        renderObjects(mGates, renderer, snappedCameraPos, const_cast<Vector&>(viewSize));

        for (auto& screw : mScrews)
        {
            if (!screw.isDestroyed() && screw.isVisible(cameraPos, viewSize))
            {
                screw.render(renderer, snappedCameraPos);
            }
        }

        for (auto& toolTip : mToolTips)
        {
            if (!toolTip.isCollected() && toolTip.isVisible(cameraPos, viewSize))
            {
                toolTip.render(renderer, snappedCameraPos);
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
            {65, 3} // Caixote
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
            
            // Configurar callbacks de som do Punktauro
            if (mPunktauro) {
                mPunktauro->setAccelerateSoundCallback(mPunktauroAccelerateSoundCallback);
                mPunktauro->setJumpSoundCallback(mPunktauroJumpSoundCallback);
                mPunktauro->setDeathSoundCallback(mPunktauroDeathSoundCallback);
            }
            
            std::cout << "PUNKTAURO BOSS spawned at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y << std::endl;
        }
        else if (strcmp(type, "gate") == 0)
        {
            processGateObject(obj, mAttributeSpawn, tileSize);
        }
        else if (strcmp(type, "tooltip_flathead") == 0)
        {
            mToolTips.emplace_back(Vector(mAttributeSpawn), ToolTipType::FLATHEAD, mToolTipsTexture, mRenderer);
            std::cout << "FLATHEAD tool tip spawned at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y << std::endl;
        }
        else if (strcmp(type, "tooltip_phillips") == 0)
        {
            mToolTips.emplace_back(Vector(mAttributeSpawn), ToolTipType::PHILLIPS, mToolTipsTexture, mRenderer);
            std::cout << "PHILLIPS tool tip spawned at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y << std::endl;
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

    void GameWorld::processGateObject(XMLElement* obj, Vector& AttributeSpawn, int tileSize)
    {
        // Obter as dimensões do portão
        float width = obj->FloatAttribute("width");
        float height = obj->FloatAttribute("height");
        
        if (width == 0) width = tileSize;
        if (height == 0) height = tileSize * 2; // Padrão: 2 tiles de altura
        
        Vector gateSize(width, height);
        
        // Processar propriedades do portão
        Vector targetScrewPos(0, 0);
        bool hasTargetScrew = false;
        
        XMLElement* properties = obj->FirstChildElement("properties");
        if (properties)
        {
            XMLElement* prop = properties->FirstChildElement("property");
            while (prop)
            {
                const char* name = prop->Attribute("name");
                if (!name) 
                {
                    prop = prop->NextSiblingElement("property");
                    continue;
                }
                
                if (strcmp(name, "target_screw_x") == 0)
                {
                    targetScrewPos.x = prop->FloatAttribute("value");
                    hasTargetScrew = true;
                }
                else if (strcmp(name, "target_screw_y") == 0)
                {
                    targetScrewPos.y = prop->FloatAttribute("value");
                }
                
                prop = prop->NextSiblingElement("property");
            }
        }
        
        // Criar o portão
        mGates.emplace_back(Vector(mAttributeSpawn), gateSize, mPlatformsTexture, mRenderer);
        
        std::cout << "Gate created at: " << mAttributeSpawn.x << ", " << mAttributeSpawn.y 
                  << " with size: " << width << "x" << height << std::endl;
        
        // Se temos coordenadas do parafuso alvo, tentar encontrá-lo
        if (hasTargetScrew)
        {
            Screw* targetScrew = findScrewByPosition(targetScrewPos, 20.0f); // 20 pixels de tolerância
            if (targetScrew)
            {
                mGates.back().setTargetScrew(targetScrew);
                std::cout << "Gate linked to screw at: " << targetScrewPos.x << ", " << targetScrewPos.y << std::endl;
            }
            else
            {
                std::cout << "Warning: Could not find target screw at: " << targetScrewPos.x << ", " << targetScrewPos.y << std::endl;
            }
        }
        else
        {
            std::cout << "Warning: Gate created without target screw specified!" << std::endl;
        }
    }

    void GameWorld::handleGateCollisions()
    {
        // Verificar colisões do player com portões fechados
        SDL_Rect playerBox = mPlayer.getHurtbox();
        
        for (auto& gate : mGates)
        {
            if (gate.blockCollision()) // Só bloqueia se não estiver aberto
            {
                SDL_Rect gateBox = gate.getBoundingBox();
                
                // Verificar sobreposição
                bool overlap =
                    (playerBox.x < gateBox.x + gateBox.w) &&
                    (playerBox.x + playerBox.w > gateBox.x) &&
                    (playerBox.y < gateBox.y + gateBox.h) &&
                    (playerBox.y + playerBox.h > gateBox.y);

                if (overlap)
                {
                    // Tratar o portão como uma plataforma sólida para empurrar o player
                    Vector playerPos = mPlayer.getPosition();
                    Vector gatePos = gate.getPosition();
                    Vector gateSize = gate.getSize();
                    
                    // Calcular qual lado está mais próximo para empurrar o player
                    float overlapLeft = (playerBox.x + playerBox.w) - gateBox.x;
                    float overlapRight = (gateBox.x + gateBox.w) - playerBox.x;
                    float overlapTop = (playerBox.y + playerBox.h) - gateBox.y;
                    float overlapBottom = (gateBox.y + gateBox.h) - playerBox.y;
                    
                    // Encontrar o menor overlap para resolver a colisão
                    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
                    
                    if (minOverlap == overlapLeft && overlapLeft > 0)
                    {
                        // Empurrar player para a esquerda
                        playerPos.x = gatePos.x - mPlayer.getSize().x;
                    }
                    else if (minOverlap == overlapRight && overlapRight > 0)
                    {
                        // Empurrar player para a direita
                        playerPos.x = gatePos.x + gateSize.x;
                    }
                    else if (minOverlap == overlapTop && overlapTop > 0)
                    {
                        // Empurrar player para cima
                        playerPos.y = gatePos.y - mPlayer.getSize().y;
                        Vector velocity = mPlayer.getVelocity();
                        if (velocity.y > 0) velocity.y = 0; // Parar queda
                        mPlayer.setVelocity(velocity);
                        mPlayer.setOnGround(true);
                    }
                    else if (minOverlap == overlapBottom && overlapBottom > 0)
                    {
                        // Empurrar player para baixo
                        playerPos.y = gatePos.y + gateSize.y;
                        Vector velocity = mPlayer.getVelocity();
                        if (velocity.y < 0) velocity.y = 0; // Parar salto
                        mPlayer.setVelocity(velocity);
                    }
                    
                    mPlayer.setPosition(playerPos);
                }
            }
        }
    }

    void GameWorld::handleToolTipCollisions()
    {
        SDL_Rect playerBox = mPlayer.getHurtbox();
        
        for (auto& toolTip : mToolTips)
        {
            if (toolTip.isCollected()) continue; // Pular se já foi coletada
            
            SDL_Rect toolTipBox = toolTip.getBoundingBox();
            
            // Verificar colisão entre player e tool tip
            bool overlap =
                (playerBox.x < toolTipBox.x + toolTipBox.w) &&
                (playerBox.x + playerBox.w > toolTipBox.x) &&
                (playerBox.y < toolTipBox.y + toolTipBox.h) &&
                (playerBox.y + playerBox.h > toolTipBox.y);

            if (overlap)
            {
                // Coletar a ponta
                toolTip.collect();
                
                // Tocar som de tooltip
                if (mTooltipSoundCallback) {
                    mTooltipSoundCallback();
                }
                
                // Converter tipo de ToolTip para AttackType
                AttackType attackType;
                if (toolTip.getType() == ToolTipType::FLATHEAD) {
                    attackType = AttackType::CUTTING;
                } else {
                    attackType = AttackType::PIERCING;
                }
                
                // Adicionar ao inventário do player
                mPlayer.collectToolTip(attackType);
                
                std::cout << "Player collected " 
                          << (toolTip.getType() == ToolTipType::FLATHEAD ? "FLATHEAD" : "PHILLIPS") 
                          << " tool tip!" << std::endl;
            }
        }
    }

    Screw* GameWorld::findScrewByPosition(const Vector& position, float tolerance)
    {
        for (auto& screw : mScrews)
        {
            Vector screwPos = screw.getPosition();
            float distance = std::sqrt(std::pow(screwPos.x - position.x, 2) + 
                                     std::pow(screwPos.y - position.y, 2));
            
            if (distance <= tolerance)
            {
                return &screw;
            }
        }
        return nullptr;
    }

    Screw* GameWorld::findScrewById(int id)
    {
        // Por enquanto, este método retorna nullptr
        // Pode ser implementado quando adicionarmos IDs aos parafusos
        return nullptr;
    }
}