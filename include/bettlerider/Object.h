#ifndef __BETTLE_RIDER_OBJECT_HEADER_H__
#define __BETTLE_RIDER_OBJECT_HEADER_H__

#include <SDL2/SDL.h>


namespace BRTC {

    class Object
    {
    public:
    Object(float x, float y, float width, float height) : mX(x), mY(y), mWidth(width), mHeight(height){}
        virtual ~Object() = default;
    
        virtual void render(SDL_Renderer *renderer, float cameraX, float cameraY) = 0;
    
        // Getters e Setters
        float getX() const      { return mX; }
        float getY() const      { return mY; }
        float getWidth() const  { return mWidth; }
        float getHeight() const { return mHeight; }
        void setX(float x)      { mX = x; }
        void setY(float y)      { mY = y; }
        bool isVisible(float cameraX, float cameraY, float screenWidth, float screenHeight);
    
    protected:
        float mX, mY;
        float mWidth, mHeight;
    
    };
    
    class StaticObject : public Object
    {
    public:
        StaticObject(float x, float y, float width, float height) 
            : Object(x, y, width, height) {}
        
        virtual ~StaticObject() = default;
    
        void render(SDL_Renderer *renderer, float cameraX, float cameraY) override = 0;
}; 

class DynamicObject : public Object
{   
public:
    DynamicObject(float x, float y, float width, float height)
    : Object(x, y, width, height) {}

    virtual ~DynamicObject() = default;
   
    virtual int getWidth() const { return mWidth; };
    virtual int getHeight() const { return mHeight; };
    virtual void update() = 0;
    virtual void render(SDL_Renderer* renderer, float cameraX, float cameraY) = 0;
    virtual void handleEvent(SDL_Event& e) = 0;
   
    float getPosX() const {return mPosX; };
    float getPosY() const {return mPosY; };
    float getHorizontalVelocity() const { return mVelX; };
    float getVerticalVelocity() const { return mVelY; };
    bool isOnGround() const { return mOnGround; };
    bool isFalling() const { return mFalling; };
    bool isPassingThroughPlatform() const { return mPassingThroughPlatform; };

    void setVelocity(float vx, float vy) {mVelX = vx; mVelY = vy;};
    void setHorizontalVelocity(float vx) {mVelX = vx; };
    void setVerticalVelocity(float vy) {mVelY = vy;};
    void setPosition(float x, float y) {mPosX = x; mPosY = y; };
    void setOnGround(bool onGround) {mOnGround = onGround; };
    void setFalling(bool falling) {mFalling = falling; };
    void setAboveCrate(bool aboveCrate) {mAboveCrate = aboveCrate; };

protected:
   float mPosX, mPosY; // Posição
   float mVelX, mVelY; // Velocidade
   int mWidth, mHeight; // Dimensões
   bool mPassingThroughPlatform; // Estado de passar por plataformas
   bool mAboveCrate; // Estado de estar acima de uma caixa
   bool mOnGround;   // Estado de estar no chão
   bool mFalling;    // Estado de cair

};

}

#endif