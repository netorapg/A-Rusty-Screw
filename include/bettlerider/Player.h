#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>

namespace BRTC {

class Player : public DynamicObject {
public:
    Player(Vector position, SDL_Renderer* renderer);
    ~Player();

    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer, Vector cameraPosition) override;
    void handleEvent(SDL_Event& e) override;
    void setPassingThroughPlatform(bool enable);
    
    bool isFacingRight() const { return mFacingRight; }
    int getWidth() const { return static_cast<int>(mSize.x); }
    int getHeight() const { return static_cast<int>(mSize.y); }

private:
    Sprite mSprite;
    bool mFacingRight;
    bool mIsJumping;
   // bool mIsAttacking;
   // bool mIsSuperAttacking;
    
    static constexpr float GRAVITY = 0.5f;
    static constexpr float JUMP_FORCE = -10.0f;
    static constexpr float MOVE_SPEED = 5.0f;
};

} // namespace BRTC
#endif