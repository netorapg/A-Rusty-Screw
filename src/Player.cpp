#include "../include/arscrew/Player.h"

namespace ARSCREW
{

  Player::Player( Vector position, SDL_Renderer *renderer )
  : DynamicObject
    ( 
      position, 
      Vector( 20, 28) 
    ), 
    mFacingDirection(1),
    mIsJumping( false )
  {
    
    SDL_Surface* surface = IMG_Load("../assets/screwer_sprites.png");
    if (!surface) 
    { throw std::runtime_error("Failed to load sprite sheet: " + std::string(IMG_GetError())); }
    spriteSheetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!spriteSheetTexture) 
    { throw std::runtime_error("Failed to create texture form sprite sheet: " + std::string(IMG_GetError()));}
  
    Animation runAnim;
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 0, 35, 37 }), 0.1f, { 0, -9 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 105, 0, 35, 37 }), 0.1f, { 0, -9 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 0, 35, 37 }), 0.1f, { 0, -9 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 175, 0, 35, 37 }), 0.1f, { 0, -9 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 37, 35, 37 }), 0.1f, { 0, -9 } } );
      runAnim.setLoop( true );

    Animation idleAnim;
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 0, 35, 37 }), 0.2f, { 0, -9 } } );
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 0, 35, 37 }), 0.2f, { 0, -9 }}  );
      idleAnim.setLoop( true );

    
    Animation cuttingAttackAnim;
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 37, 35, 37 }), 0.09f, { 0, -9 } } );
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 178, 53, 40, 27 }), 0.01f, { 0, 0 } } );
      cuttingAttackAnim.setLoop( false );

    
    Animation piercingAttackAnim;
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 74, 36, 37 }), 0.1f, { 0, -9 } } );
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 270, 84, 56, 27 }), 0.1f, { 2, 0 } } );
      piercingAttackAnim.setLoop( false );

    Animation jumpAnim;
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 37, 35, 37 }), 0.1f, { 0, -9 } } );
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 37, 35, 37 }), 0.1f, { 0, -9 } } );
      jumpAnim.setLoop( true );
    
    Animation JumpCuttingAttackAnim;
        JumpCuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 232, 50, 40, 30 }), 0.1f, { 0, 1 } } );
        JumpCuttingAttackAnim.setLoop( false );

    Animation JumpPiercingAttackAnim;
        JumpPiercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 281, 52, 53, 26 }), 0.1f, { 0, 1 } } );
        JumpPiercingAttackAnim.setLoop( false );

    animations["run"] = runAnim;
    animations["idle"] = idleAnim;
    animations["jump"] = jumpAnim;
    animations["cuttingAttack"] = cuttingAttackAnim;
    animations["piercingAttack"] = piercingAttackAnim;
    animations["jumpCuttingAttack"] = JumpCuttingAttackAnim;
    animations["jumpPiercingAttack"] = JumpPiercingAttackAnim;
    currentAnimation = "idle";
    
    // Inicializar com a ponta FLATHEAD por padrão
    mCollectedToolTips.insert(AttackType::CUTTING);
    mCurrentAttackType = AttackType::CUTTING;
    std::cout << "Player initialized with FLATHEAD tool tip (CUTTING attacks enabled)" << std::endl;
}

void Player::moveLeft()
{
    if (!mIsDashing) {
        Vector velocity = getVelocity();
        velocity.x = -MOVE_SPEED;
        mFacingDirection = -1;
        mIsMovingLeft = true;
        setVelocity(velocity);
    }
}

void Player::moveRight()
{
    if (!mIsDashing) {
        Vector velocity = getVelocity();
        velocity.x = MOVE_SPEED;
        mFacingDirection = 1;
        mIsMovingRight = true;
        setVelocity(velocity);
    }
}

void Player::stopHorizontalMovement()
{
    if (!mIsDashing) {
        Vector velocity = getVelocity();
        velocity.x = 0;
        mIsMovingLeft = false;
        mIsMovingRight = false;
        setVelocity(velocity);
    }
}

void Player::setMovementInput(bool movingLeft, bool movingRight)
{
    mIsMovingLeft = movingLeft;
    mIsMovingRight = movingRight;
    
    if (!mIsDashing) {
        Vector velocity = getVelocity();
        
        if (movingLeft && !movingRight) {
            velocity.x = -MOVE_SPEED;
            mFacingDirection = -1;
        } else if (movingRight && !movingLeft) {
            velocity.x = MOVE_SPEED;
            mFacingDirection = 1;
        } else {
            velocity.x = 0;
        }
        
        setVelocity(velocity);
    }
}

void Player::jump()
{
    Vector velocity = getVelocity();
    
    if (isOnGround()) {
        velocity.y = JUMP_FORCE;
        mIsJumping = true;
    }
    
    handleWallJump(velocity);
}

void Player::startAttack()
{
    if (!mIsAttacking) {
        // Verificar se o jogador pode usar o tipo de ataque atual
        if (!canUseAttackType(mCurrentAttackType)) {
            std::cout << "Cannot attack! Missing " 
                      << (mCurrentAttackType == AttackType::CUTTING ? "FLATHEAD" : "PHILLIPS") 
                      << " tool tip!" << std::endl;
            return;
        }
        
        mIsAttacking = true;
        mAttackDuration = ATTACK_DURATION;
        updateAttackHitbox();
        
        std::cout << "Attacking with " 
                  << (mCurrentAttackType == AttackType::CUTTING ? "CUTTING" : "PIERCING") 
                  << " attack!" << std::endl;
    }
}

void Player::startDash()
{
    // Condições mais específicas para o dash
    if (!mIsDashing && isOnGround() && !isCollidingWithWall()) {
        mIsDashing = true;
        mDashTimer = DASH_DURATION;
        std::cout << "Dash initiated! Direction: " << mFacingDirection << std::endl;
    }
}

void Player::toggleDebugDisplay()
{
    mShowDebugRects = !mShowDebugRects;
}

void Player::passThroughPlatform(bool enable)
{
    setPassingThroughPlatform(enable);
}

void Player::switchAttackType()
{
    // Verificar se o jogador pode trocar para o próximo tipo de ataque
    if (mCurrentAttackType == AttackType::CUTTING) {
        if (hasToolTip(AttackType::PIERCING)) {
            mCurrentAttackType = AttackType::PIERCING;
            std::cout << "Switched to PIERCING attack (for Phillips screws)" << std::endl;
        } else {
            std::cout << "Cannot switch to PIERCING attack - Phillips tool tip not collected!" << std::endl;
        }
    } else {
        if (hasToolTip(AttackType::CUTTING)) {
            mCurrentAttackType = AttackType::CUTTING;
            std::cout << "Switched to CUTTING attack (for Flathead screws)" << std::endl;
        } else {
            std::cout << "Cannot switch to CUTTING attack - Flathead tool tip not collected!" << std::endl;
        }
    }
}

int Player::getAttackDamage() const
{
    // Dano base para ataque cortante (CUTTING)
    if (mCurrentAttackType == AttackType::CUTTING) {
        return 25;
    }
    // Dano maior para ataque perfurante (PIERCING)
    else if (mCurrentAttackType == AttackType::PIERCING) {
        return 35;
    }
    
    return 25; // Fallback para dano padrão
}

void Player::handleWallJump(Vector& velocity)
{
    if (isCollidingWithWall() && mFacingDirection == 1)
    {
        velocity.x = -MOVE_SPEED;
        velocity.y = JUMP_FORCE;
        mFacingDirection = -1;
        setIsCollidingWithWall(false);
    }
    else if (isCollidingWithWall() && mFacingDirection == -1)
    {
        velocity.x = MOVE_SPEED;
        velocity.y = JUMP_FORCE;
        mFacingDirection = 1;
        setIsCollidingWithWall(false);
    }
    setVelocity(velocity);
}

void Player::update(float deltaTime) 
{
    Vector velocity = getVelocity();
    Vector position = getPosition();

    // Decrementar buffer de colisão lateral
    if (mLateralCollisionBuffer > 0.0f) {
        mLateralCollisionBuffer -= deltaTime;
    }
    
    // Resetar flag de colisão lateral do frame anterior
    mHadLateralCollisionThisFrame = false;
    
    // Armazenar posição antes do movimento
    mLastValidPosition = position;

    // Atualizar invulnerabilidade
    updateInvulnerability(deltaTime);

    // Lógica do dash
    if (mIsDashing) {
        mDashTimer -= deltaTime;
        if (mFacingDirection == 1) {
            velocity.x = 0.0f;
            velocity.x += DASH_SPEED;
            position += velocity * deltaTime;
        } else if (mFacingDirection == -1) {
            velocity.x = 0.0f;
            velocity.x += -DASH_SPEED;
            position += velocity * deltaTime;
        }
        if (mDashTimer <= 0.0f) {
            mIsDashing = false;
            velocity.x = 0.0f;
        }
    }

    // Aplicar gravidade
    velocity.y += GRAVITY * deltaTime;

    // Aplicar movimento vertical primeiro
    Vector verticalMovement(0, velocity.y * deltaTime);
    position += verticalMovement;

    // Aplicar movimento horizontal com verificação preventiva de colisão
    if (!mIsDashing) {
        Vector horizontalMovement(velocity.x * deltaTime, 0);
        
        // Se há buffer de colisão lateral ativo e está tentando se mover na direção da parede,
        // reduzir significativamente o movimento para evitar penetração visual
        if (mLateralCollisionBuffer > 0.0f) {
            if ((velocity.x > 0 && mFacingDirection == 1) || (velocity.x < 0 && mFacingDirection == -1)) {
                // Reduzir movimento para quase zero se insistindo na direção da parede
                horizontalMovement.x *= 0.05f;
            }
        }
        
        position += horizontalMovement;
    }

    // Verificar se está no chão
    if (isOnGround()) {
        setIsCollidingWithWall(false);
        mIsJumping = false;
    
        // REMOVER a verificação do teclado e substituir por:
        if (!mIsMovingLeft && !mIsMovingRight && !mIsDashing) {
            velocity.x = 0.0f;
        }
    } else {
        mIsJumping = true;
    }

    // Lógica de animação
    std::string newAnimation;
    if (isOnGround()) {
        if (mIsAttacking) {
            if (mCurrentAttackType == AttackType::CUTTING) {
                newAnimation = "cuttingAttack";
            } else {
                newAnimation = "piercingAttack";
            }
        } else if (velocity.x != 0.0f || mIsDashing) {
            newAnimation = "run";
        } else {
            newAnimation = "idle";
        }
    } else if (mIsAttacking && mCurrentAttackType == AttackType::CUTTING && mIsJumping) {
        newAnimation = "jumpCuttingAttack";
    } else if (mIsAttacking && mCurrentAttackType == AttackType::PIERCING && mIsJumping) {
        newAnimation = "jumpPiercingAttack";
    } else {
        newAnimation = "jump";
    }

    if (newAnimation != currentAnimation) {
        animations[newAnimation].reset();
        currentAnimation = newAnimation;
    }

    animations[currentAnimation].update(deltaTime);

    // Aplicar mudanças
    setVelocity(velocity);
    setPosition(position);

    updateHurtbox();

    // Atualizar ataque
    if (mIsAttacking) {
        mAttackDuration -= deltaTime;
        if (mAttackDuration <= 0.0f) {
            mIsAttacking = false;
        }
    }
}

void Player::takeDamage(int damage)
{
    // Só toma dano se não estiver invulnerável
    if (mInvulnerabilityTimer <= 0.0f && !isDead())
    {
        mCurrentHealth -= damage;
        if (mCurrentHealth < 0) {
            mCurrentHealth = 0;
        }
        
        // Ativar invulnerabilidade
        mInvulnerabilityTimer = INVULNERABILITY_DURATION;
        mIsFlashing = true;
        mFlashTimer = 0.0f;
        
        std::cout << "Player took " << damage << " damage! Health: " 
                  << mCurrentHealth << "/" << mMaxHealth << std::endl;
        
        if (isDead()) {
            std::cout << "Player died!" << std::endl;
            // Aqui você pode adicionar lógica de morte (reiniciar level, game over, etc.)
        }
        
        // Opcional: Adicionar knockback
        Vector velocity = getVelocity();
        velocity.x = (mFacingDirection == 1) ? -150.0f : 150.0f; // Knockback na direção oposta
        if (isOnGround()) {
            velocity.y = -100.0f; // Pequeno pulo para cima
        }
        setVelocity(velocity);
    }
}

void Player::heal(int healAmount)
{
    if (!isDead()) {
        mCurrentHealth += healAmount;
        if (mCurrentHealth > mMaxHealth) {
            mCurrentHealth = mMaxHealth;
        }
        
        std::cout << "Player healed " << healAmount << " HP! Health: " 
                  << mCurrentHealth << "/" << mMaxHealth << std::endl;
    }
}

void Player::resetHealth()
{
    mCurrentHealth = mMaxHealth;
    mInvulnerabilityTimer = 0.0f;
    mIsFlashing = false;
    mFlashTimer = 0.0f;
    
    std::cout << "Player health reset to maximum: " 
              << mCurrentHealth << "/" << mMaxHealth << std::endl;
}

void Player::updateInvulnerability(float deltaTime)
{
    if (mInvulnerabilityTimer > 0.0f) {
        mInvulnerabilityTimer -= deltaTime;
        
        // Atualizar efeito de piscar
        if (mIsFlashing) {
            mFlashTimer += deltaTime;
            if (mFlashTimer >= FLASH_INTERVAL) {
                mFlashTimer = 0.0f;
            }
        }
        
        if (mInvulnerabilityTimer <= 0.0f) {
            mIsFlashing = false;
            mFlashTimer = 0.0f;
        }
    }
}

void Player::render(SDL_Renderer* renderer, Vector cameraPosition) 
{
    Vector screenPos = getPosition() - cameraPosition;

    if (mShowHurtbox && mShowDebugRects) {
        SDL_Rect hurtboxOnScreen = {
            mHurtbox.x - static_cast<int>(cameraPosition.x),
            mHurtbox.y - static_cast<int>(cameraPosition.y),
            mHurtbox.w,
            mHurtbox.h
        };
        
        DrawDebugOutline(renderer, hurtboxOnScreen.x, hurtboxOnScreen.y, 
                        hurtboxOnScreen.w, hurtboxOnScreen.h, 255, 255, 0, 2);
    }

    if (mIsAttacking && mShowAttackHitbox && mShowDebugRects) {
        SDL_Rect attackRectOnScreen = {
            mAttackHitbox.x - static_cast<int>(cameraPosition.x),
            mAttackHitbox.y - static_cast<int>(cameraPosition.y),
            mAttackHitbox.w,
            mAttackHitbox.h
        };

        if (mCurrentAttackType == AttackType::CUTTING) {
            DrawDebugOutline(renderer, attackRectOnScreen.x, attackRectOnScreen.y, 
                           attackRectOnScreen.w, attackRectOnScreen.h, 0, 255, 0, 2);
        } else {
            DrawDebugOutline(renderer, attackRectOnScreen.x, attackRectOnScreen.y, 
                           attackRectOnScreen.w, attackRectOnScreen.h, 0, 0, 255, 2);
        }
    }

    // Só renderizar se não estiver piscando ou se estiver na fase visível do piscar
    bool shouldRender = !mIsFlashing || (mFlashTimer < FLASH_INTERVAL / 2);
    
    if (shouldRender) {
        SpritePtr currentSprite = animations[currentAnimation].getCurrentSprite();
        
        if(currentSprite) 
        {
            SDL_Point baseOffset = *animations[currentAnimation].getCurrentOffset();
            const SDL_Rect& frameRect = currentSprite->getSrcRect();
            float baseWidth = 20;
            float frameWidth = frameRect.w;
            float widthDifference = frameWidth - baseWidth;
            Vector renderOffset;
            renderOffset.x = static_cast<int>(screenPos.x);
            renderOffset.y = static_cast<int>(screenPos.y);
            if(mFacingDirection == -1) 
            { renderOffset.x -= widthDifference * 1.0f; } 
            else 
            { renderOffset.x += widthDifference * 0.0f; }
            renderOffset.x += baseOffset.x;
            renderOffset.y += baseOffset.y;
            
            if (mShowDebugRects) 
            {
                DrawDebugOutline
                (
                    renderer, 
                    static_cast<int>(screenPos.x), 
                    static_cast<int>(screenPos.y), 
                    getWidth(), 
                    getHeight(), 
                    255, 0, 0, 1
                );
            }
            
            // Se estiver invulnerável, renderizar com cor vermelha
            if (mInvulnerabilityTimer > 0.0f) {
                SDL_SetTextureColorMod(spriteSheetTexture, 255, 100, 100);
            } else {
                SDL_SetTextureColorMod(spriteSheetTexture, 255, 255, 255);
            }
            
            currentSprite->draw(renderer, renderOffset.x, renderOffset.y, mFacingDirection == -1);
        }
    }
}

  void Player::setPassingThroughPlatform( bool enable )
  {
    mPassingThroughPlatform = enable;
  }

void Player::updateHurtbox()
{
    Vector pos = getPosition();
    
    // Hurtbox é um pouco menor que o sprite para gameplay mais justo
    mHurtbox.x = static_cast<int>(pos.x + 2);
    mHurtbox.y = static_cast<int>(pos.y + 2);
    mHurtbox.w = static_cast<int>(mSize.x - 4);
    mHurtbox.h = static_cast<int>(mSize.y - 4);
    
    // Atualizar hitbox de ataque quando estiver atacando
    if (mIsAttacking)
    {
        updateAttackHitbox();
    }
}

void Player::updateAttackHitbox()
{
    Vector pos = getPosition();
    
    if (mCurrentAttackType == AttackType::CUTTING)
    {
        // Ataque cortante - mais largo e menor alcance
        if (mFacingDirection == 1) // Direita
        {
            mAttackHitbox.x = static_cast<int>(pos.x + mSize.x);
            mAttackHitbox.y = static_cast<int>(pos.y + 5);
            mAttackHitbox.w = 20;
            mAttackHitbox.h = 20;
        }
        else // Esquerda
        {
            mAttackHitbox.x = static_cast<int>(pos.x - mSize.x);
            mAttackHitbox.y = static_cast<int>(pos.y + 5);
            mAttackHitbox.w = 20;
            mAttackHitbox.h = 20;
        }
    }
    else // PIERCING
    {
        // Ataque perfurante - mais estreito e maior alcance
        if (mFacingDirection == 1) // Direita
        {
            mAttackHitbox.x = static_cast<int>(pos.x + mSize.x);
            mAttackHitbox.y = static_cast<int>(pos.y + 8);
            mAttackHitbox.w = 35;
            mAttackHitbox.h = 15;
        }
        else // Esquerda
        {
            mAttackHitbox.x = static_cast<int>(pos.x - 30);
            mAttackHitbox.y = static_cast<int>(pos.y + 8);
            mAttackHitbox.w = 30;
            mAttackHitbox.h = 15;
        }
    }
}

void Player::DrawDebugOutline(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int thickness)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    
    for (int i = 0; i < thickness; i++)
    {
        SDL_Rect outline;
        
        // Top
        outline = {x - i, y - i, w + 2*i, thickness};
        SDL_RenderFillRect(renderer, &outline);
        
        // Bottom
        outline = {x - i, y + h - thickness + i, w + 2*i, thickness};
        SDL_RenderFillRect(renderer, &outline);
        
        // Left
        outline = {x - i, y - i, thickness, h + 2*i};
        SDL_RenderFillRect(renderer, &outline);
        
        // Right
        outline = {x + w - thickness + i, y - i, thickness, h + 2*i};
        SDL_RenderFillRect(renderer, &outline);
    }
}

void Player::collectToolTip(AttackType tipType)
{
    if (mCollectedToolTips.find(tipType) == mCollectedToolTips.end()) {
        mCollectedToolTips.insert(tipType);
        std::cout << "Collected " << (tipType == AttackType::CUTTING ? "FLATHEAD" : "PHILLIPS") 
                  << " tool tip! Can now use " 
                  << (tipType == AttackType::CUTTING ? "CUTTING" : "PIERCING") 
                  << " attacks." << std::endl;
        
        // Se o jogador não possui nenhuma ponta e coletou uma, trocar para ela
        if (mCollectedToolTips.size() == 1) {
            mCurrentAttackType = tipType;
            std::cout << "Automatically switched to " 
                      << (tipType == AttackType::CUTTING ? "CUTTING" : "PIERCING") 
                      << " attack type!" << std::endl;
        }
    } else {
        std::cout << "Already have " << (tipType == AttackType::CUTTING ? "FLATHEAD" : "PHILLIPS") 
                  << " tool tip!" << std::endl;
    }
}

bool Player::hasToolTip(AttackType tipType) const
{
    return mCollectedToolTips.find(tipType) != mCollectedToolTips.end();
}

bool Player::canUseAttackType(AttackType attackType) const
{
    // O jogador pode usar um tipo de ataque se possui a ponta correspondente
    return hasToolTip(attackType);
}

void Player::switchToAvailableAttackType()
{
    // Se o tipo atual não está disponível, trocar para o primeiro disponível
    if (!canUseAttackType(mCurrentAttackType)) {
        for (AttackType type : mCollectedToolTips) {
            mCurrentAttackType = type;
            std::cout << "Auto-switched to " 
                      << (type == AttackType::CUTTING ? "CUTTING" : "PIERCING") 
                      << " attack type!" << std::endl;
            return;
        }
        
        // Se chegou aqui, o jogador não tem nenhuma ponta
        std::cout << "Warning: Player has no tool tips collected!" << std::endl;
    }
}

}
