#ifndef __WEAPON_H__
#define __WEAPON_H__

#include <unordered_map>
#include <memory>
#include "Object.h"
#include "Animation.h"


namespace ARSCREW
{
    class Weapon : public DynamicObject
    {
    public:
        Weapon(Vector position, Vector size, SDL_Renderer* renderer, const std::string& texturePath);
        ~Weapon();
        
        void update(float deltaTime) override;
        void render(SDL_Renderer* renderer, Vector cameraPosition) override;
        void handleEvent(SDL_Event& e) override {}
        
        void setAnchor(Object* anchor, Vector localOffset);
        void setFacingDirection(int direction) { mFacingDirection = direction; }
        
    private:
        Object* mAnchor = nullptr;
        Vector mLocalOffset;
        int mFacingDirection = 1;
        std::unordered_map<std::string, Animation> mAnimations;
        std::string mCurrentAnimation;
    };
}

#endif