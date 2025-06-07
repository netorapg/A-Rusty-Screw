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
        , mFollowSpeed(DEFAULT_FOLLOW_SPEED)
        , mFollowDistance(DEFAULT_FOLLOW_DISTANCE)
        , mLastPlayerPosition(position)
        , mCurrentAnimation("idle")
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
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{0, 16, 16, 16}), 0.2f, {0, 0}});
        walkAnim.addFrame({std::make_shared<Sprite>(mSpriteSheetTexture, SDL_Rect{16, 16, 16, 16}), 0.2f, {0, 0}});
        idleAnim.setLoop(true);

        mAnimations["idle"] = idleAnim;
        mAnimations["walk"] = walkAnim;

    }

    void Chicken::handleEvent(SDL_Event& e)
    {
        // Currently no specific events to handle for Chicken
        // This can be extended later if needed
    }

    void Chicken::update(float deltaTime)
    {
        Vector velocity = getVelocity();
        Vector position = getPosition();

        velocity.y += GRAVITY * deltaTime;
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

        static int debugCounter = 0;
        if (++debugCounter % 60 == 0)
        {
            std::cout << "Chicken followPlayer: "
                      << "Player Position: (" << playerPosition.x << ", " << playerPosition.y << "), "
                      << "Chicken Position: (" << chickenPosition.x << ", " << chickenPosition.y << "), "
                      << "Distance: " << distance << std::endl;
        }

        if (distance < STOP_DISTANCE)
        {
            Vector velocity = getVelocity();
            velocity.x = 0.0f;
            setVelocity(velocity);
            mIsMoving = false;
            return;
        }

        if (distance > mFollowDistance)
        {
            float normalizedDirectionX = direction.x / distance;
            float speedMultiplier = std::min(distance / mFollowDistance, 3.0f);
            Vector velocity = getVelocity();
            velocity.x =  normalizedDirectionX * mFollowSpeed * speedMultiplier;
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
                          << "Velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
            }
        }
        else
        {
            Vector velocity = getVelocity();
            velocity.x = 0.0f;
            setVelocity(velocity);
            mIsMoving = false;
        }

        mLastPlayerPosition = playerPosition;
    }

    void Chicken::updateAnimation()
    {
        std::string newAnimation;

        if (mIsMoving && std::abs(getVelocity().x) > 10.0f)
        {
            newAnimation = "walk";
        } 
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