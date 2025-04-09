#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "Animation.h"
#include "Sprite.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>

#include "Globals.h"

namespace BRTC {

class Player : public DynamicObject {
    SDL_Texture* spriteSheetTexture;
public:
    Player(Vector position, SDL_Renderer* renderer);
    ~Player() {
        if (spriteSheetTexture) {
            SDL_DestroyTexture(spriteSheetTexture);
        }
    }

    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
    void handleEvent(SDL_Event& e) override;
    void setPassingThroughPlatform(bool enable);
    
    bool isFacingRight() const { return mFacingRight; }
    int getWidth() const { return static_cast<int>(mSize.x); }
    int getHeight() const { return static_cast<int>(mSize.y); }

private:
    bool mFacingRight;
    bool mIsJumping;
    bool mIspunching = false;
    bool mIspunchingHarder = false;
    bool mIsFalling = false;
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;
    
    
    
};

} // namespace BRTC
#endif