#ifndef PLAYER_H
#define PLAYER_H

#include "Object.h"
#include "Sprite.h"
#include <SDL2/SDL.h>
#include "Globals.h"

namespace BRTC {

class Player : public DynamicObject {
public:
    Player(Vector position, SDL_Renderer* renderer);
    ~Player();

    void update() override;
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
    bool mIspunching = false;
    bool mIspunchingHarder = false;
    
    
    
};

} // namespace BRTC
#endif