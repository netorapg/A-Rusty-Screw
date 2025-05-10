#include <iostream>
#include "../include/bettlerider/Weapon.h"

namespace BRTC
{
    Weapon::Weapon(Vector position, Vector size, SDL_Renderer* renderer, const std::string& texturePath)
        : DynamicObject(position, size)
        {
            auto weaponSprite = std::make_shared<Sprite>(renderer, texturePath);

            Animation idleAnim;
            AnimationFrame frame;
            frame.sprite = weaponSprite;
            frame.duration = 0.1f;
            frame.offset = { 0, 0 };
            idleAnim.addFrame(frame);
            idleAnim.setLoop(true);

            mAnimations["idle"] = idleAnim;
            mCurrentAnimation = "idle";
        }

    Weapon::~Weapon() {
        mAnimations.clear();
    }

    void Weapon::update(float deltaTime) {
        if (mAnchor) {
            mPosition = mAnchor->getPosition() + mLocalOffset;
        }

        mAnimations[mCurrentAnimation].update(deltaTime);
    }

    void Weapon::render(SDL_Renderer* renderer, Vector cameraPosition) {
        Vector screenPos = mPosition - cameraPosition;
        auto currentSprite = mAnimations[mCurrentAnimation].getCurrentSprite();

        if (currentSprite) {
            currentSprite->draw(renderer,
                static_cast<int>(screenPos.x),
                static_cast<int>(screenPos.y),
                mFacingDirection == -1
            );
        }
    }

    void Weapon::setAnchor(Object* anchor, Vector localOffset) {
        mAnchor = anchor;
        mLocalOffset = localOffset;
    }
}

