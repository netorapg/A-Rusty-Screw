#include "../include/arscrew/Punktauro.h"
#include "../include/arscrew/Globals.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

namespace ARSCREW
{
    // Definição das constantes
    const float Punktauro::SPECIAL_ATTACK_COOLDOWN = 3.0f;
    const float Punktauro::CHARGE_SPEED = 200.0f;
    const int Punktauro::PHASE_1_DAMAGE = 15;
    const int Punktauro::PHASE_2_DAMAGE = 20;
    const int Punktauro::PHASE_3_DAMAGE = 25;

    Punktauro::Punktauro(Vector position, SDL_Renderer* renderer)
        : Enemy(position, renderer), mCurrentState(PunktauroState::PHASE_1)
    {
        // Carregar textura específica do boss (substitui a textura do Enemy)
        if (mSpriteSheetTexture) {
            SDL_DestroyTexture(mSpriteSheetTexture);
        }
        
        // Tente carregar uma textura específica do boss primeiro
        mSpriteSheetTexture = IMG_LoadTexture(renderer, "../assets/punktauro.png");
        
        
        // Configurações específicas do boss
        mMaxHealth = 200;
        mCurrentHealth = mMaxHealth;
        mPhase1Health = mMaxHealth * 0.7f;  // 140 HP
        mPhase2Health = mMaxHealth * 0.3f;  // 60 HP
        
        // Configurações de ataque
        mDamage = PHASE_1_DAMAGE;
        mFollowSpeed = 100.0f;
        mFollowDistance = 250.0f;
        mAttackDistance = 80.0f;
        
        // Inicializar variáveis específicas
        mSpecialAttackTimer = 0.0f;
        mSpecialAttackCooldown = SPECIAL_ATTACK_COOLDOWN;
        mIsDoingSpecialAttack = false;
        mOriginalPosition = position;
        mMovementTimer = 0.0f;
        mIsCharging = false;
        
        // Tamanho maior para o boss
        mSize = Vector(73, 50);
        
        // Recarregar animações com a nova textura
        loadBossAnimations(renderer);
        
        std::cout << "Punktauro boss criado com " << mMaxHealth << " de vida!" << std::endl;
    }

    Punktauro::~Punktauro()
    {
        // Destructor
    }

    void Punktauro::update(float deltaTime)
    {
        if (mIsDestroyed || isDead()) 
        {
            mCurrentState = PunktauroState::DEFEATED;
            return;
        }
        
        // Aplicar física básica (gravidade e movimento) - IMPORTANTE!
        Vector velocity = getVelocity();
        Vector position = getPosition();
        
        // Aplica gravidade se não estiver no chão
        velocity.y += GRAVITY * deltaTime;
        
        // Atualiza posição baseada na velocidade
        position += velocity * deltaTime;
        
        // Aplica mudanças de posição e velocidade
        setVelocity(velocity);
        setPosition(position);
        
        // Atualizar fase baseado na vida
        updatePhase();
        
        // Atualizar timers
        mSpecialAttackTimer -= deltaTime;
        mMovementTimer += deltaTime;
        
        // Atualizar sistemas do boss
        updateAnimation(deltaTime);
        updateInvulnerability(deltaTime);
        updateAttack(deltaTime);
        updatePunktauroHitboxes();
    }

    void Punktauro::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        if (mIsDestroyed || isDead()) return;
        
        Vector screenPos = getPosition() - cameraPosition;

        // Desenha hitboxes de debug
        if (mShowDebugRects)
        {
            // Hurtbox (verde para o boss - corpo completo)
            SDL_Rect hurtboxOnScreen = {
                mHurtbox.x - static_cast<int>(cameraPosition.x),
                mHurtbox.y - static_cast<int>(cameraPosition.y),
                mHurtbox.w,
                mHurtbox.h
            };
            DrawDebugRect(renderer, hurtboxOnScreen.x, hurtboxOnScreen.y, 
                         hurtboxOnScreen.w, hurtboxOnScreen.h, 0, 255, 0);

            // Hurtbox da cabeça (amarelo para destacar)
            SDL_Rect headHurtboxOnScreen = {
                mHeadHurtbox.x - static_cast<int>(cameraPosition.x),
                mHeadHurtbox.y - static_cast<int>(cameraPosition.y),
                mHeadHurtbox.w,
                mHeadHurtbox.h
            };
            DrawDebugRect(renderer, headHurtboxOnScreen.x, headHurtboxOnScreen.y,
                         headHurtboxOnScreen.w, headHurtboxOnScreen.h, 255, 255, 0);

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
            // Renderiza sprite usando o mesmo padrão do Enemy base
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

                    // Aplicar efeito de flash baseado na fase e invulnerabilidade
                    if (mInvulnerabilityTimer > 0.0f) {
                        // Vermelho quando invulnerável
                        SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 100, 100);
                    } else {
                        // Cores diferentes para cada fase
                        switch (mCurrentState)
                        {
                            case PunktauroState::PHASE_1:
                                SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 255, 255); // Normal
                                break;
                            case PunktauroState::PHASE_2:
                                SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 200, 100); // Laranja
                                break;
                            case PunktauroState::PHASE_3:
                                SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 100, 200); // Rosa/Roxo
                                break;
                            default:
                                SDL_SetTextureColorMod(mSpriteSheetTexture, 255, 255, 255);
                                break;
                        }
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

    void Punktauro::updateWithPlayer(const Player& player, float deltaTime)
    {
        if (mIsDestroyed || isDead()) return;
        
        // Comportamento baseado na fase atual
        switch (mCurrentState)
        {
            case PunktauroState::PHASE_1:
                phase1Behavior(player, deltaTime);
                break;
            case PunktauroState::PHASE_2:
                phase2Behavior(player, deltaTime);
                break;
            case PunktauroState::PHASE_3:
                phase3Behavior(player, deltaTime);
                break;
            case PunktauroState::DEFEATED:
                // Boss foi derrotado
                break;
        }
        
        // Atualizar direção que está olhando
        Vector playerPos = player.getPosition();
        if (playerPos.x < mPosition.x)
            mFacingDirection = -1;
        else
            mFacingDirection = 1;
    }

    void Punktauro::takeDamage(int damage)
    {
        if (isInvulnerable() || isDead()) return;
        
        mCurrentHealth -= damage;
        mInvulnerabilityTimer = INVULNERABILITY_DURATION;
        
        std::cout << "Punktauro levou " << damage << " de dano! Vida: " << mCurrentHealth << "/" << mMaxHealth << std::endl;
        
        if (mCurrentHealth <= 0)
        {
            mCurrentHealth = 0;
            mCurrentState = PunktauroState::DEFEATED;
            mIsDestroyed = true;
            std::cout << "Punktauro foi derrotado!" << std::endl;
        }
    }

    void Punktauro::updatePhase()
    {
        if (mCurrentHealth > mPhase1Health && mCurrentState != PunktauroState::PHASE_1)
        {
            // Retornar para fase 1 (cura?)
            mCurrentState = PunktauroState::PHASE_1;
            mDamage = PHASE_1_DAMAGE;
            mFollowSpeed = 60.0f;
            std::cout << "Punktauro voltou para FASE 1" << std::endl;
        }
        else if (mCurrentHealth <= mPhase1Health && mCurrentHealth > mPhase2Health && mCurrentState == PunktauroState::PHASE_1)
        {
            // Entrar na fase 2
            mCurrentState = PunktauroState::PHASE_2;
            mDamage = PHASE_2_DAMAGE;
            mFollowSpeed = 80.0f;
            std::cout << "Punktauro entrou na FASE 2!" << std::endl;
        }
        else if (mCurrentHealth <= mPhase2Health && mCurrentState != PunktauroState::PHASE_3)
        {
            // Entrar na fase 3
            mCurrentState = PunktauroState::PHASE_3;
            mDamage = PHASE_3_DAMAGE;
            mFollowSpeed = 100.0f;
            std::cout << "Punktauro entrou na FASE 3 - FASE FINAL!" << std::endl;
        }
    }

    void Punktauro::phase1Behavior(const Player& player, float deltaTime)
    {
        // Fase 1: Comportamento básico de seguir e atacar
        followPlayer(player, deltaTime);
        
        // Ataque especial ocasional
        if (mSpecialAttackTimer <= 0.0f && !mIsAttacking)
        {
            performSpecialAttack(player, deltaTime);
            mSpecialAttackTimer = mSpecialAttackCooldown;
        }
    }

    void Punktauro::phase2Behavior(const Player& player, float deltaTime)
    {
        // Fase 2: Mais agressivo, ataques mais frequentes
        followPlayer(player, deltaTime);
        
        // Ataques especiais mais frequentes
        if (mSpecialAttackTimer <= 0.0f && !mIsAttacking)
        {
            performSpecialAttack(player, deltaTime);
            mSpecialAttackTimer = mSpecialAttackCooldown * 0.7f; // 30% mais rápido
        }
        
        // Padrão de movimento especial
        updateMovementPattern(player, deltaTime);
    }

    void Punktauro::phase3Behavior(const Player& player, float deltaTime)
    {
        // Fase 3: Muito agressivo, ataques constantes
        if (!mIsCharging)
        {
            followPlayer(player, deltaTime);
        }
        
        // Ataques especiais muito frequentes
        if (mSpecialAttackTimer <= 0.0f && !mIsAttacking)
        {
            performSpecialAttack(player, deltaTime);
            mSpecialAttackTimer = mSpecialAttackCooldown * 0.5f; // 50% mais rápido
        }
        
        // Padrão de movimento agressivo
        updateMovementPattern(player, deltaTime);
    }

    void Punktauro::performSpecialAttack(const Player& player, float deltaTime)
    {
        Vector playerPos = player.getPosition();
        Vector direction = playerPos - mPosition;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance < 150.0f) // Se o player estiver próximo
        {
            // Iniciar ataque especial
            mIsDoingSpecialAttack = true;
            mIsAttacking = true;
            mAttackDuration = ATTACK_DURATION * 1.5f; // Ataque mais longo
            mCurrentAnimation = "attack";
            
            std::cout << "Punktauro executou ataque especial!" << std::endl;
        }
    }

    void Punktauro::updateMovementPattern(const Player& player, float deltaTime)
    {
        // Padrão de movimento baseado no timer
        if (mCurrentState == PunktauroState::PHASE_2 || mCurrentState == PunktauroState::PHASE_3)
        {
            // A cada 4 segundos, fazer uma investida
            if (static_cast<int>(mMovementTimer) % 4 == 0 && mMovementTimer - static_cast<int>(mMovementTimer) < deltaTime)
            {
                mIsCharging = true;
            }
            
            if (mIsCharging)
            {
                Vector playerPos = player.getPosition();
                Vector chargeDirection = playerPos - mPosition;
                float distance = std::sqrt(chargeDirection.x * chargeDirection.x + chargeDirection.y * chargeDirection.y);
                
                if (distance > 5.0f)
                {
                    chargeDirection.x /= distance;
                    chargeDirection.y /= distance;
                    
                    mVelocity.x = chargeDirection.x * CHARGE_SPEED;
                    mVelocity.y = chargeDirection.y * CHARGE_SPEED;
                    
                    mCurrentAnimation = "walk";
                }
                else
                {
                    mIsCharging = false;
                    mVelocity.x = 0;
                    mVelocity.y = 0;
                }
            }
        }
    }

    void Punktauro::updatePunktauroHitboxes()
    {
        Vector pos = getPosition();
        
        // Atualizar hurtbox padrão (corpo completo) - chamando método da classe pai
        updateHitboxes();
        
        // Configurar hurtbox específica da cabeça do Punktauro
        // A cabeça está localizada na parte superior do sprite
        // Assumindo que a cabeça ocupa aproximadamente os primeiros 30% da altura
        float headHeightPercent = 0.3f;
        int headHeight = static_cast<int>(mSize.y * headHeightPercent);
        
        // A cabeça tem uma largura um pouco menor que o corpo
        float headWidthPercent = 0.7f;
        int headWidth = static_cast<int>(mSize.x * headWidthPercent);
        
        // Centralizar a hurtbox da cabeça horizontalmente
        int headOffsetX = static_cast<int>((mSize.x - headWidth) / 2);
        
        mHeadHurtbox.x = static_cast<int>(pos.x) + headOffsetX;
        mHeadHurtbox.y = static_cast<int>(pos.y); // Topo do sprite
        mHeadHurtbox.w = headWidth;
        mHeadHurtbox.h = headHeight;
    }

    void Punktauro::loadBossAnimations(SDL_Renderer* renderer)
    {
        // Limpar animações existentes
        mAnimations.clear();
        
        // Animações específicas do boss (ajuste os retângulos conforme sua sprite)
        
        // Animação idle do boss
        Animation idleAnim;
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{13, 14, 73, 50}), 0.6f, {0, 0}});
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{93, 16, 72, 48}), 0.6f, {0, 0}});
        idleAnim.setLoop(true);
        mAnimations["idle"] = idleAnim;
        
        // Animação de caminhada do boss
        Animation walkAnim;
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{13, 14, 73, 50}), 0.3f, {0, 0}});
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{93, 16, 72, 48}), 0.3f, {0, 0}});
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{174, 17, 73, 47}), 0.3f, {0, 0}});
        walkAnim.setLoop(true);
        mAnimations["walk"] = walkAnim;
        
        // Animação de ataque do boss
        Animation attackAnim;
      //  attackAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{0, 128, 64, 64}), 0.2f, {0, 0}});
       // attackAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{64, 128, 64, 64}), 0.3f, {0, 0}});
      //  attackAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{128, 128, 64, 64}), 0.2f, {0, 0}});
      //  attackAnim.setLoop(false);
        mAnimations["attack"] = attackAnim;
        
        // Definir animação inicial
        mCurrentAnimation = "idle";
        
        std::cout << "Animações do boss Punktauro carregadas!" << std::endl;
    }
}