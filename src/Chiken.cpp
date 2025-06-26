#include "../include/arscrew/Chicken.h"
#include "../include/arscrew/Player.h"
#include <cmath>

namespace ARSCREW
{
    Chicken::Chicken(Vector position, SDL_Renderer* renderer)
        : DynamicObject(position, Vector(16, 16))
        , mSpriteSheetTexture(nullptr)
        , mFacingDirection(1)
        , mIsMoving(false)
        , mFollowSpeed(120.0f)  // Força o valor diretamente
        , mFollowDistance(50.0f)  // Força o valor diretamente
        , mLastPlayerPosition(position)
        , mCurrentAnimation("idle")
        , mIsFlying(false)
    {
        SDL_Surface* surface = IMG_Load("../assets/Anselma.png");
        if (!surface)
        {
            std::cerr << "Error loading chicken sprite sheet: " << IMG_GetError() << std::endl;
        }
        else
        {
            mSpriteSheetTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (!mSpriteSheetTexture)
            {
                std::cerr << "Failed to create chicken texture: " << SDL_GetError() << std::endl;
            }
        }

        std::cout << "Chicken created with follow speed: " << mFollowSpeed << std::endl;

        loadAnimations();
    }

    Chicken::~Chicken()
    {
        if (mSpriteSheetTexture)
        {
            SDL_DestroyTexture(mSpriteSheetTexture);

        }
    }

    void Chicken::loadAnimations()
    {
        if (!mSpriteSheetTexture) 
        {
            Animation idleAnim;
            idleAnim.addFrame({nullptr, 0.5f, {0, 0}});
            idleAnim.setLoop(true);

            Animation walkAnim;
            walkAnim.addFrame({nullptr, 0.2f, {0, 0}});
            walkAnim.setLoop(true);

            mAnimations["idle"] = idleAnim;
            mAnimations["walk"] = walkAnim;
            return;
        }

        Animation idleAnim;
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{0, 0, 16, 16}), 0.5f, {0, 0}});
        idleAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{16, 0, 16, 16}), 0.5f, {0, 0}});
        idleAnim.setLoop(true);

        Animation walkAnim;
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{112, 0, 16, 16}), 0.2f, {0, 0}});
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{128, 0, 16, 16}), 0.2f, {0, 0}});
        idleAnim.setLoop(true);

        Animation flyAnim;
        flyAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{80, 0, 16, 16}), 0.2f, {0, 0}});
        flyAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{96, 0, 16, 16}), 0.2f, {0, 0}});
        mAnimations["idle"] = idleAnim;
        mAnimations["walk"] = walkAnim;
        mAnimations["fly"] = flyAnim;

    }

    void Chicken::update(float deltaTime)
    {
        Vector velocity = getVelocity();
        Vector position = getPosition();

        if (!mIsFlying)
        {
            velocity.y += GRAVITY * deltaTime; // Gravity effect
        }
        position += velocity * deltaTime;

        setVelocity(velocity);
        setPosition(position);

        updateAnimation();
        mAnimations[mCurrentAnimation].update(deltaTime);
    }

    void Chicken::followPlayer(const Player& player, float deltaTime)
    {
        Vector playerPosition = player.getPosition();
        Vector playerCenter = Vector(
            playerPosition.x + player.getSize().x / 2,
            playerPosition.y + player.getSize().y / 2
        );

        Vector chickenPosition = getPosition();
        Vector chickenCenter = Vector(
            chickenPosition.x + getSize().x / 2,
            chickenPosition.y + getSize().y / 2
        );

        Vector direction = playerCenter - chickenCenter;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        bool playerInAir = !player.isOnGround();

        static int debugCounter = 0;
        if (++debugCounter % 60 == 0)
        {
            std::cout << "Chicken followPlayer: "
                      << "Player Position: (" << playerPosition.x << ", " << playerPosition.y << "), "
                      << "Chicken Position: (" << chickenPosition.x << ", " << chickenPosition.y << "), "
                      << "Distance: " << distance << ", "
                      << "Follow Speed: " << mFollowSpeed << std::endl;
        }

        if (distance < STOP_DISTANCE)
        {
            Vector velocity = getVelocity();
            velocity.x = 0.0f;
            if (playerInAir)
            {
                velocity.y = 0.0f; // Stop vertical movement if player is in air
                mIsFlying = true; // Set flying state if player is in air
            }
            else
            {
                mIsFlying = false; // Reset flying state if player is on ground
            }
            
            setVelocity(velocity);
            mIsMoving = false;
            return;
        }

        if (distance > mFollowDistance)
        {
            // Verificar se a distância é válida para evitar divisão por zero
            if (distance > 0.1f)
            {
                float normalizedDirectionX = direction.x / distance;
               
                float speedMultiplier = std::min(distance / mFollowDistance, 3.0f);
                
                // Usar valores fixos para debug
                float targetVelocityX = normalizedDirectionX * 120.0f * speedMultiplier; // Força 120 como velocidade base
                
                Vector velocity = getVelocity();
                velocity.x = targetVelocityX;
                if (playerInAir)
                {
                    float normalizedDirectionY = direction.y / distance;
                    float targetVelocityY = normalizedDirectionY * 120.0f * speedMultiplier; // Força 120 como velocidade base
                    velocity.y = targetVelocityY;
                    mIsFlying = true; // Set flying state if player is in air
                }
                else 
                {
                    mIsFlying = false; // Reset flying state if player is on ground
                }
                setVelocity(velocity);

                if (direction.x > 0)
                    mFacingDirection = 1;
                else if (direction.x < 0)
                    mFacingDirection = -1;

                mIsMoving = true;

                if (debugCounter % 60 == 0)
                {
                    std::cout << "Chicken moving towards player: "
                              << "Direction: (" << direction.x << ", " << direction.y << "), "
                              << "Normalized X: " << normalizedDirectionX << ", "
                              << "Speed multiplier: " << speedMultiplier << ", "
                              << "Target Velocity X: " << targetVelocityX << ", "
                              << "Final Velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
                }
            }
        }
        else
        {
            Vector velocity = getVelocity();
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            setVelocity(velocity);
            mIsMoving = false;
        }

        mLastPlayerPosition = playerPosition;
    }

    void Chicken::updateAnimation()
    {
        std::string newAnimation;

        // Se a galinha está voando (não no chão), usa animação de voar
        if (mIsFlying)
        {
            newAnimation = "fly";
        }
        // Se está se movendo no chão, usa animação de andar
        else if (mIsMoving && std::abs(getVelocity().x) > 10.0f)
        {
            newAnimation = "walk";
        } 
        // Se está parada no chão, usa animação idle
        else 
        {
            newAnimation = "idle";
        }

        if (newAnimation != mCurrentAnimation)
        {
            mAnimations[newAnimation].reset();
            mCurrentAnimation = newAnimation;
        }
    }

    void Chicken::render(SDL_Renderer* renderer, Vector cameraPosition)
    {
        Vector screenPos = getPosition() - cameraPosition;

        if (!mSpriteSheetTexture)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect chickenRect = {
                static_cast<int>(screenPos.x),
                static_cast<int>(screenPos.y),
                static_cast<int>(getSize().x),
                static_cast<int>(getSize().y)
            };
            SDL_RenderFillRect(renderer, &chickenRect);
            return;
        }

        SpritePtr currentSprite = mAnimations[mCurrentAnimation].getCurrentSprite();

        if (currentSprite)
        {
            SDL_Point baseOffset = *mAnimations[mCurrentAnimation].getCurrentOffset();
            Vector renderOffset;
            renderOffset.x = static_cast<int>(screenPos.x + baseOffset.x);
            renderOffset.y = static_cast<int>(screenPos.y + baseOffset.y);
            currentSprite->draw(renderer, renderOffset.x, renderOffset.y, mFacingDirection == -1);
        }
    }
}