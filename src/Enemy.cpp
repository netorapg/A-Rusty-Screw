#include "../include/arscrew/Enemy.h"
#include "../include/arscrew/Globals.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

namespace ARSCREW
{
    Enemy::Enemy(Vector position, SDL_Renderer* renderer)
        : DynamicObject(position, Vector(25, 29))
        , mSpriteSheetTexture(nullptr)
        , mCurrentState(EnemyState::IDLE)
        , mFacingDirection(1)
        , mFollowSpeed(80.0f)
        , mFollowDistance(150.0f)
        , mAttackDistance(30.0f)
        , mCurrentAnimation("idle")
        , mIsAttacking(false)
        , mIsDestroyed(false)  
        , mDamage(10)          
        , mAttackDuration(0.0f)
        , mAttackCooldown(0.0f)
        , mShowDebugRects(false)
    {
        // Carrega a textura do inimigo (você pode usar a mesma do player por enquanto)
        mSpriteSheetTexture = IMG_LoadTexture(renderer, "../assets/enemy_sprite.png");
        if (!mSpriteSheetTexture)
        {
            std::cout << "Erro ao carregar textura do inimigo: " << SDL_GetError() << std::endl;
        }

        loadAnimations(renderer);
        updateHitboxes();
    }

    Enemy::~Enemy()
    {
        if (mSpriteSheetTexture)
        {
            SDL_DestroyTexture(mSpriteSheetTexture);
        }
    }

    void Enemy::loadAnimations(SDL_Renderer* renderer)
    {
        // Animação idle
        Animation idleAnim;
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{0, 3, 25, 29}), 0.5f, {0, 0}});
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{32, 3, 25, 28}), 0.5f, {0, 0}});
        idleAnim.setLoop(true);
        mAnimations["idle"] = idleAnim;

        // Animação de corrida
        Animation runAnim;
        runAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{0, 3, 25, 29}), 0.1f, {0, 0}});
        runAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{32, 3, 25, 28}), 0.1f, {0, 0}});
        runAnim.setLoop(true);
        mAnimations["run"] = runAnim;

        // Animação de ataque
        Animation attackAnim;
        attackAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{32, 35, 30, 29}), 0.15f, {0, 1}});
        attackAnim.setLoop(false);
        mAnimations["attack"] = attackAnim;
    }

    void Enemy::updateWithPlayer(const Player& player, float deltaTime)
    {
        updateState(player, deltaTime);
        update(deltaTime);
    }
 void Enemy::takeDamage(int damage)
    {
        if (mInvulnerabilityTimer <= 0.0f && !isDead())
        {
            mCurrentHealth -= damage;
            if (mCurrentHealth < 0) {
                mCurrentHealth = 0;
            }

            mInvulnerabilityTimer = INVULNERABILITY_DURATION;
            mIsFlashing = true; // Inicia o efeito de piscar
            mFlashTimer = 0.0f; // Reseta o temporizador de pis
            
            // Resetar flags de som para permitir novos sons
            mHitSoundPlayed = false;
            mDeathSoundPlayed = false;

            std::cout << "Enemy took damage: " << damage 
                      << ", Current Health: " << mCurrentHealth << std::endl;
            if (isDead())
            {
                std::cout << "Enemy died!" << std::endl;
                destroy();
            }
        }
    }

    void Enemy::heal(int healAmount)
    {
        if (!isDead()) {
            mCurrentHealth += healAmount;
            if (mCurrentHealth > mMaxHealth) {
                mCurrentHealth = mMaxHealth;
            }

            std::cout << "Enemy healed " << healAmount << " HP! Health: " 
                      << mCurrentHealth << "/" << mMaxHealth << std::endl;
        }
    }

    void Enemy::updateInvulnerability(float deltaTime)
    {
        if (mInvulnerabilityTimer > 0.0f) {
            mInvulnerabilityTimer -= deltaTime;

            if (mIsFlashing) {
                mFlashTimer += deltaTime;
                if (mFlashTimer >= FLASH_INTERVAL) {
                    mFlashTimer = 0.0f;
                }
            }

            if (mInvulnerabilityTimer <= 0.0f) {
                mIsFlashing = false; // Desativa o efeito de piscar
                mFlashTimer = 0.0f; // Reseta o temporizador de piscar
            }
        }
    }


    void Enemy::update(float deltaTime)
    {
        Vector velocity = getVelocity();
        Vector position = getPosition();

        updateInvulnerability(deltaTime);

        // Aplica gravidade
        velocity.y += GRAVITY * deltaTime;

        // Atualiza posição
        position += velocity * deltaTime;

        // Atualiza estado interno
        updateAnimation(deltaTime);
        updateAttack(deltaTime);
        updateHitboxes();

        // Aplica mudanças
        setVelocity(velocity);
        setPosition(position);
    }

    void Enemy::updateState(const Player& player, float deltaTime)
    {
        Vector playerPos = player.getPosition();
        Vector enemyPos = getPosition();
        
        // Calcula distância até o player
        Vector direction = playerPos - enemyPos;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Determina direção que deve encarar
        if (direction.x > 0)
            mFacingDirection = 1;
        else if (direction.x < 0)
            mFacingDirection = -1;

        // Máquina de estados
        switch (mCurrentState)
        {
            case EnemyState::IDLE:
                if (distance <= mFollowDistance)
                {
                    mCurrentState = EnemyState::FOLLOWING;
                }
                break;

            case EnemyState::FOLLOWING:
                if (distance > mFollowDistance)
                {
                    mCurrentState = EnemyState::IDLE;
                }
                else if (distance <= mAttackDistance && mAttackCooldown <= 0.0f)
                {
                    mCurrentState = EnemyState::ATTACKING;
                    mIsAttacking = true;
                    mAttackDuration = ATTACK_DURATION;
                    mAttackCooldown = ATTACK_COOLDOWN;
                }
                else
                {
                    followPlayer(player, deltaTime);
                }
                break;

            case EnemyState::ATTACKING:
                if (mAttackDuration <= 0.0f)
                {
                    mIsAttacking = false;
                    if (distance <= mFollowDistance)
                        mCurrentState = EnemyState::FOLLOWING;
                    else
                        mCurrentState = EnemyState::IDLE;
                }
                break;

            case EnemyState::STUNNED:
                // Reduzir timer de stun
                mStunTimer -= deltaTime;
                if (mStunTimer <= 0.0f)
                {
                    mStunTimer = 0.0f;
                    mCurrentState = EnemyState::IDLE; // Voltar para idle quando stun acabar
                }
                break;
        }
    }

    void Enemy::followPlayer(const Player& player, float deltaTime)
    {
        Vector playerPos = player.getPosition();
        Vector enemyPos = getPosition();
        Vector direction = playerPos - enemyPos;
        
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance > 0.1f && distance > mAttackDistance)
        {
            // Normaliza a direção
            direction.x /= distance;
            
            Vector velocity = getVelocity();
            velocity.x = direction.x * mFollowSpeed;
            setVelocity(velocity);
        }
    }

    void Enemy::updateAnimation(float deltaTime)
    {
        std::string newAnimation;

        switch (mCurrentState)
        {
            case EnemyState::IDLE:
                newAnimation = "idle";
                break;
            case EnemyState::FOLLOWING:
                newAnimation = "run";
                break;
            case EnemyState::ATTACKING:
                newAnimation = "attack";
                break;
            case EnemyState::STUNNED:
                newAnimation = "idle";
                break;
        }

        // Troca animação se necessário
        if (newAnimation != mCurrentAnimation)
        {
            if (mAnimations.find(newAnimation) != mAnimations.end())
            {
                mAnimations[newAnimation].reset();
                mCurrentAnimation = newAnimation;
            }
        }

        // Atualiza animação atual
        if (mAnimations.find(mCurrentAnimation) != mAnimations.end())
        {
            mAnimations[mCurrentAnimation].update(deltaTime);
        }
    }

    void Enemy::updateAttack(float deltaTime)
    {
        if (mAttackCooldown > 0.0f)
        {
            mAttackCooldown -= deltaTime;
        }

        if (mIsAttacking && mAttackDuration > 0.0f)
        {
            mAttackDuration -= deltaTime;
        }
    }

    void Enemy::updateHitboxes()
    {
        Vector pos = getPosition();
        
        // Hurtbox (caixa de colisão para receber dano)
        mHurtbox.x = static_cast<int>(pos.x);
        mHurtbox.y = static_cast<int>(pos.y);
        mHurtbox.w = static_cast<int>(mSize.x);
        mHurtbox.h = static_cast<int>(mSize.y);

        // Attack hitbox (caixa de ataque)
        if (mIsAttacking)
        {
            int hitboxWidth = 20;
            int hitboxHeight = 25;
            
            if (mFacingDirection == 1) // Direita
            {
                mAttackHitbox.x = static_cast<int>(pos.x + mSize.x);
                mAttackHitbox.y = static_cast<int>(pos.y + mSize.y / 2 - hitboxHeight / 2);
            }
            else // Esquerda
            {
                mAttackHitbox.x = static_cast<int>(pos.x - hitboxWidth);
                mAttackHitbox.y = static_cast<int>(pos.y + mSize.y / 2 - hitboxHeight / 2);
            }
            
            mAttackHitbox.w = hitboxWidth;
            mAttackHitbox.h = hitboxHeight;
        }
    }

    SDL_Rect Enemy::getBoundingBox() const
    {
        SDL_Rect box;
        box.x = static_cast<int>(getPosition().x);
        box.y = static_cast<int>(getPosition().y);
        box.w = static_cast<int>(mSize.x);
        box.h = static_cast<int>(mSize.y);
        return box;
    }


void Enemy::render(SDL_Renderer* renderer, Vector cameraPosition)
{
    Vector screenPos = getPosition() - cameraPosition;

    // Desenha hitboxes de debug
    if (mShowDebugRects)
    {
        // Hurtbox (azul)
        SDL_Rect hurtboxOnScreen = {
            mHurtbox.x - static_cast<int>(cameraPosition.x),
            mHurtbox.y - static_cast<int>(cameraPosition.y),
            mHurtbox.w,
            mHurtbox.h
        };
        DrawDebugRect(renderer, hurtboxOnScreen.x, hurtboxOnScreen.y, 
                     hurtboxOnScreen.w, hurtboxOnScreen.h, 0, 0, 255);

        // Attack hitbox (vermelho)
        if (mIsAttacking)
        {
            SDL_Rect attackboxOnScreen = {
                mAttackHitbox.x - static_cast<int>(cameraPosition.x),
                mAttackHitbox.y - static_cast<int>(cameraPosition.y),
                mAttackHitbox.w,
                mAttackHitbox.h
            };
            DrawDebugRect(renderer, attackboxOnScreen.x, attackboxOnScreen.y,
                     attackboxOnScreen.w, attackboxOnScreen.h, 255, 0, 0);
        }
    }

    // Só renderizar se não estiver piscando ou se estiver na fase visível do piscar
    bool shouldRender = !mIsFlashing || (mFlashTimer < FLASH_INTERVAL / 2);
    
    if (shouldRender) {
        // Renderiza sprite usando o mesmo padrão do Player
        if (mAnimations.find(mCurrentAnimation) != mAnimations.end())
        {
            SpritePtr currentSprite = mAnimations[mCurrentAnimation].getCurrentSprite();
            if (currentSprite)
            {
                const SDL_Point* offset = mAnimations[mCurrentAnimation].getCurrentOffset();
                SDL_Point finalOffset = offset ? *offset : SDL_Point{0, 0};

                // Calcula posição de renderização
                Vector renderPos;
                renderPos.x = screenPos.x + finalOffset.x;
                renderPos.y = screenPos.y + finalOffset.y;

                // Se estiver invulnerável, renderizar com cor vermelha
                if (mInvulnerabilityTimer > 0.0f) {
                    SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 100, 100);
                } else {
                    SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 255, 255);
                }

                // Usa o método draw da classe Sprite
                bool shouldFlip = (mFacingDirection == -1);
                currentSprite->draw(renderer, 
                              static_cast<int>(renderPos.x), 
                              static_cast<int>(renderPos.y), 
                              shouldFlip);
            }
        }
    }
}

    void Enemy::DrawDebugRect(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
    {
        SDL_SetRenderDrawColor(renderer, r, g, b, 100);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

