#include "../include/arscrew/Player.h"

namespace ARSCREW
{

  Player::Player( Vector position, SDL_Renderer *renderer )
  : DynamicObject
    ( 
      position, 
      Vector( 20, 37) 
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
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 105, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 175, 0, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      runAnim.setLoop( true );

    Animation idleAnim;
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 0, 35, 37 }), 0.2f, { 0, 0 } } );
      idleAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 0, 35, 37 }), 0.2f, { 0, 0 }}  );
      idleAnim.setLoop( true );

    
    Animation cuttingAttackAnim;
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 140, 37, 35, 37 }), 0.09f, { 0, 0 } } );
      cuttingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 178, 53, 40, 27 }), 0.01f, { 0, 10 } } );
      cuttingAttackAnim.setLoop( false );

    
    Animation piercingAttackAnim;
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 0, 74, 36, 37 }), 0.1f, { 0, 0 } } );
      piercingAttackAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 270, 84, 56, 27 }), 0.1f, { 0, 10 } } );
      piercingAttackAnim.setLoop( false );

    Animation jumpAnim;
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 35, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      jumpAnim.addFrame( { std::make_shared<Sprite>(spriteSheetTexture, SDL_Rect{ 70, 37, 35, 37 }), 0.1f, { 0, 0 } } );
      jumpAnim.setLoop( true );

    animations["run"] = runAnim;
    animations["idle"] = idleAnim;
    animations["jump"] = jumpAnim;
    animations["cuttingAttack"] = cuttingAttackAnim;
    animations["piercingAttack"] = piercingAttackAnim;
    currentAnimation = "idle";
}

void Player::handleEvent( SDL_Event &e )
{
    Vector velocity = getVelocity();
    
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        switch( e.key.keysym.sym )
        {
            case SDLK_d:
                if (!mIsDashing) { // Só mover se não estiver dashando
                    velocity.x = MOVE_SPEED;
                    mFacingDirection = 1;
                }
                break;
            case SDLK_a:
                if (!mIsDashing) { // Só mover se não estiver dashando
                    velocity.x = -MOVE_SPEED;
                    mFacingDirection = -1;
                }
                break;
            case SDLK_SPACE:
                if(isOnGround())
                {
                    velocity.y = JUMP_FORCE;
                    mIsJumping = true;
                }
                handleWallJump(velocity);
                break;
            case SDLK_s:
                if( isOnGround() ) {
                    setPassingThroughPlatform(true);
                }
                break;
            case SDLK_j:
                if (!mIsAttacking) {
                    mIsAttacking = true;
                    mAttackDuration = ATTACK_DURATION;
                    updateAttackHitbox();
                }
                break;
            case SDLK_q:
                switchAttackType();
                break;
            case SDLK_e:
                // Condições mais específicas para o dash
                if (!mIsDashing && isOnGround() && !isCollidingWithWall()) {
                    mIsDashing = true;
                    mDashTimer = DASH_DURATION;
                    std::cout << "Dash initiated! Direction: " << mFacingDirection << std::endl;
                }
                break;
            case SDLK_LCTRL:
                mShowDebugRects = !mShowDebugRects;
                break;
        }
    }
    else if( e.type == SDL_KEYUP )
    {
        switch( e.key.keysym.sym )
        {
            case SDLK_a:
            case SDLK_d:
                if (!mIsDashing) { // Só parar se não estiver dashando
                    velocity.x = 0;
                }
                break;
            case SDLK_s:
                setPassingThroughPlatform(false);
                break;
        }
    }
    
    setVelocity( velocity );
}

void Player::switchAttackType()
{
    if (mCurrentAttackType == AttackType::CUTTING) {
        mCurrentAttackType = AttackType::PIERCING;
        std::cout << "Switched to PIERCING attack (for Phillips screws)" << std::endl;
    } else {
        mCurrentAttackType = AttackType::CUTTING;
        std::cout << "Switched to CUTTING attack (for Flathead screws)" << std::endl;
    }
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

    // Atualizar posição
    position += velocity * deltaTime;

    // Verificar se está no chão
    if (isOnGround()) {
        setIsCollidingWithWall(false);
        mIsJumping = false;
    
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]) {
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
            mAttackHitbox.w = 30;
            mAttackHitbox.h = 25;
        }
        else // Esquerda
        {
            mAttackHitbox.x = static_cast<int>(pos.x - 30);
            mAttackHitbox.y = static_cast<int>(pos.y + 5);
            mAttackHitbox.w = 30;
            mAttackHitbox.h = 25;
        }
    }
    else // PIERCING
    {
        // Ataque perfurante - mais estreito e maior alcance
        if (mFacingDirection == 1) // Direita
        {
            mAttackHitbox.x = static_cast<int>(pos.x + mSize.x);
            mAttackHitbox.y = static_cast<int>(pos.y + 8);
            mAttackHitbox.w = 40;
            mAttackHitbox.h = 15;
        }
        else // Esquerda
        {
            mAttackHitbox.x = static_cast<int>(pos.x - 40);
            mAttackHitbox.y = static_cast<int>(pos.y + 8);
            mAttackHitbox.w = 40;
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

}
