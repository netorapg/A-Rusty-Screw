#include "../include/arscrew/Punktauro.h"
#include "../include/arscrew/Globals.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cmath>

namespace ARSCREW
{
    // Definição das constantes
    const float Punktauro::SPECIAL_ATTACK_COOLDOWN = 4.0f;
    const float Punktauro::CHARGE_SPEED = 500.0f;
    const int Punktauro::PHASE_1_DAMAGE = 15;
    const int Punktauro::PHASE_2_DAMAGE = 20;
    const int Punktauro::PHASE_3_DAMAGE = 25;
    const float Punktauro::BOSS_INVULNERABILITY_DURATION = 3.0f; // 2 segundos para o boss

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
        mMaxHealth = 300;
        mCurrentHealth = mMaxHealth;
        mPhase1Health = mMaxHealth * 0.7f;  // 140 HP
        mPhase2Health = mMaxHealth * 0.3f;  // 60 HP
        
        // Configurações de ataque
        mDamage = PHASE_1_DAMAGE;
        mFollowSpeed = 300.0f;
        mFollowDistance = 250.0f;
        mAttackDistance = 80.0f;
        
        // Inicializar variáveis específicas
        mSpecialAttackTimer = 0.0f;
        mSpecialAttackCooldown = SPECIAL_ATTACK_COOLDOWN;
        mIsDoingSpecialAttack = false;
        mOriginalPosition = position;
        mMovementTimer = 0.0f;
        mIsCharging = false;
        mDeathSoundPlayed = false; // Inicializar flag de som de morte
        mAccelerateSoundPlayed = false; // Inicializar flag de som de aceleração
        mJumpSoundPlayed = false; // Inicializar flag de som de pulo
        
        // Tamanho maior para o boss
        mSize = Vector(70, 70);
        
        // Configurar escala visual (pode ser ajustada para fazer o boss parecer maior)
        // 1.0f = tamanho original, 1.5f = 50% maior, 2.0f = 100% maior (dobro do tamanho)
        mVisualScale = 2.0f; // 50% maior que o tamanho original
        
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
        
        // Atualizar fase baseado na vida
        updatePhase();
        
        // Resetar flag de som de pulo quando volta ao chão
        if (isOnGround() && mJumpSoundPlayed) {
            mJumpSoundPlayed = false;
        }
        
        // Atualizar timers
        mSpecialAttackTimer -= deltaTime;
        mMovementTimer += deltaTime;
        
        // Atualizar sistemas do boss
        updateAnimation(deltaTime);
        updateInvulnerability(deltaTime);
        updateAttack(deltaTime);
        updatePunktauroHitboxes();
        
        // Chamar o update da classe base Enemy para aplicar física
        Enemy::update(deltaTime);
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

                    // Usa renderização escalada customizada em vez do método draw padrão
                    bool shouldFlip = (mFacingDirection == -1);
                    
                    // Calcular tamanho escalado
                    SDL_Point spriteSize = currentSprite->getSize();
                    int scaledWidth = static_cast<int>(spriteSize.x * mVisualScale);
                    int scaledHeight = static_cast<int>(spriteSize.y * mVisualScale);
                    
                    // Ajustar posição para centralizar o sprite escalado
                    int centeredX = static_cast<int>(renderPos.x) - (scaledWidth - spriteSize.x) / 2;
                    int centeredY = static_cast<int>(renderPos.y) - (scaledHeight - spriteSize.y) / 2;
                    
                    // Criar retângulo de destino escalado
                    SDL_Rect destRect = { centeredX, centeredY, scaledWidth, scaledHeight };
                    SDL_RendererFlip flipFlag = shouldFlip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                    
                    // Renderizar com escala
                    const SDL_Rect& srcRect = currentSprite->getSrcRect();
                    SDL_RenderCopyEx(renderer, currentSprite->getTexture(), &srcRect, &destRect, 0.0, nullptr, flipFlag);
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
            
        // Chamar o método update para aplicar física
        update(deltaTime);
    }

    void Punktauro::takeDamage(int damage)
    {
        if (isInvulnerable() || isDead()) return;
        
        mCurrentHealth -= damage;
        mInvulnerabilityTimer = BOSS_INVULNERABILITY_DURATION; // Usar duração específica do boss
        mIsFlashing = true; // Ativar efeito de piscar durante invulnerabilidade
        mFlashTimer = 0.0f; // Resetar timer de flash
        
        std::cout << "Punktauro levou " << damage << " de dano! Vida: " << mCurrentHealth << "/" << mMaxHealth 
                  << " (invulnerável por " << BOSS_INVULNERABILITY_DURATION << "s)" << std::endl;
        
        if (mCurrentHealth <= 0)
        {
            mCurrentHealth = 0;
            mCurrentState = PunktauroState::DEFEATED;
            mIsDestroyed = true;
            
            // Tocar som de morte do Punktauro apenas uma vez
            if (!mDeathSoundPlayed && mDeathSoundCallback) {
                mDeathSoundCallback();
                mDeathSoundPlayed = true; // Marcar que já tocou o som
            }
            
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
        // Boss só pode fazer padrões de movimento se estiver no chão
        if (!isOnGround()) {
            // Se não estiver no chão, não faz charge nem movimentos especiais
            mIsCharging = false;
            Vector velocity = getVelocity();
            velocity.x = 0; // Para o movimento horizontal
            setVelocity(velocity);
            return;
        }
        
        // Padrão de movimento baseado no timer (apenas quando no chão)
        if (mCurrentState == PunktauroState::PHASE_2 || mCurrentState == PunktauroState::PHASE_3)
        {
            // A cada 4 segundos, fazer uma investida
            if (static_cast<int>(mMovementTimer) % 4 == 0 && mMovementTimer - static_cast<int>(mMovementTimer) < deltaTime)
            {
                mIsCharging = true;
                
                // Tocar som de aceleração apenas uma vez por charge
                if (!mAccelerateSoundPlayed && mAccelerateSoundCallback) {
                    mAccelerateSoundCallback();
                    mAccelerateSoundPlayed = true;
                }
                
                std::cout << "Punktauro iniciou charge attack!" << std::endl;
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
                    mAccelerateSoundPlayed = false; // Reset da flag quando o charge termina
                    mVelocity.x = 0;
                    mVelocity.y = 0;
                }
            }
        }
    }

    void Punktauro::updatePunktauroHitboxes()
    {
        Vector pos = getPosition();
        
        // Calcular tamanho visual escalado para hitboxes
        Vector scaledSize = mSize * mVisualScale;
        
        // A sprite escalada é centralizada visualmente, então as hitboxes devem seguir a mesma lógica
        // Posição da sprite escalada (mesma lógica do render)
        Vector hitboxPos = pos;
        hitboxPos.x -= (scaledSize.x - mSize.x) / 2;
        hitboxPos.y -= (scaledSize.y - mSize.y) / 2;
        
        // Atualizar hurtbox padrão (corpo completo) com tamanho escalado
        mHurtbox.x = static_cast<int>(hitboxPos.x);
        mHurtbox.y = static_cast<int>(hitboxPos.y);
        mHurtbox.w = static_cast<int>(scaledSize.x);
        mHurtbox.h = static_cast<int>(scaledSize.y);
        
        // Configurar hurtbox específica da cabeça do Punktauro
        // Usar percentuais menores para a hurtbox da cabeça para ser mais preciso
        float headHeightPercent = 0.2f;  // Reduzido de 0.3f para ser mais preciso
        float headWidthPercent = 0.2f;    // Reduzido de 0.7f para ser mais preciso
        
        int headHeight = static_cast<int>(scaledSize.y * headHeightPercent);
        int headWidth = static_cast<int>(scaledSize.x * headWidthPercent);
        
        // Centralizar a hurtbox da cabeça horizontalmente
        int headOffsetX = static_cast<int>((scaledSize.x - headWidth) / 2);
        
        // Posição da cabeça: início da sprite + pequeno offset para evitar bordas
        int headOffsetY = static_cast<int>(scaledSize.y * 0.05f); // 5% do topo
        
        mHeadHurtbox.x = static_cast<int>(hitboxPos.x) + headOffsetX;
        mHeadHurtbox.y = static_cast<int>(hitboxPos.y) + headOffsetY;
        mHeadHurtbox.w = headWidth;
        mHeadHurtbox.h = headHeight;
        
        // Atualizar attack hitbox se estiver atacando
        if (mIsAttacking)
        {
            int hitboxWidth = static_cast<int>(30 * mVisualScale);
            int hitboxHeight = static_cast<int>(20 * mVisualScale);
            
            if (mFacingDirection == 1) // Direita
            {
                mAttackHitbox.x = static_cast<int>(hitboxPos.x + scaledSize.x);
                mAttackHitbox.y = static_cast<int>(hitboxPos.y + scaledSize.y / 2 - hitboxHeight / 2);
            }
            else // Esquerda
            {
                mAttackHitbox.x = static_cast<int>(hitboxPos.x - hitboxWidth);
                mAttackHitbox.y = static_cast<int>(hitboxPos.y + scaledSize.y / 2 - hitboxHeight / 2);
            }
            
            mAttackHitbox.w = hitboxWidth;
            mAttackHitbox.h = hitboxHeight;
        }
    }

    void Punktauro::followPlayer(const Player& player, float deltaTime)
    {
        // Boss só pode seguir o jogador se estiver no chão
        if (!isOnGround()) {
            // Se não estiver no chão, não move horizontalmente
            Vector velocity = getVelocity();
            velocity.x = 0; // Para o movimento horizontal
            setVelocity(velocity);
            return;
        }
        
        // Se estiver no chão, usa o comportamento normal de seguir
        Vector playerPos = player.getPosition();
        Vector bossPos = getPosition();
        Vector direction = playerPos - bossPos;
        
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance > 0.1f && distance > mAttackDistance)
        {
            // Normaliza a direção horizontal
            direction.x /= distance;
            
            Vector velocity = getVelocity();
            velocity.x = direction.x * mFollowSpeed;
            
            // Pular ocasionalmente quando seguindo o jogador (especialmente nas fases 2 e 3)
            static float jumpTimer = 0.0f;
            jumpTimer += deltaTime;
            
            bool shouldJump = false;
            if (mCurrentState == PunktauroState::PHASE_2 && jumpTimer > 3.0f) {
                shouldJump = true;
                jumpTimer = 0.0f;
            } else if (mCurrentState == PunktauroState::PHASE_3 && jumpTimer > 2.0f) {
                shouldJump = true;
                jumpTimer = 0.0f;
            }
            
            if (shouldJump && velocity.y >= 0) { // Só pula se não estiver já pulando
                velocity.y = -250.0f; // Força do pulo
                
                // Tocar som de pulo do Punktauro apenas uma vez por pulo
                if (!mJumpSoundPlayed && mJumpSoundCallback) {
                    mJumpSoundCallback();
                    mJumpSoundPlayed = true;
                }
                
                std::cout << "Punktauro pulou!" << std::endl;
            }
            
            setVelocity(velocity);
        }
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