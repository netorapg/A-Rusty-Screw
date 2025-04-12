#ifndef __BETTLE_RIDER_OBJECT_HEADER_H__
#define __BETTLE_RIDER_OBJECT_HEADER_H__

#include "Globals.h"

namespace BRTC 
{
    class Object
    {
        public:
            Object(const Vector position, const Vector size) : 
            mPosition(position), mSize(size){} 
            virtual ~Object() = default;
            virtual void render(SDL_Renderer* renderer, Vector cameraPosition) = 0;
            void setPosition(Vector position) { mPosition = position; }
            Vector getPosition() const { return mPosition; }
            Vector getSize() const { return mSize; }
            bool isVisible(Vector cameraPosition, Vector screenSize) const
            {
                return 
                (
                    mPosition.x < cameraPosition.x + screenSize.x &&
                    mPosition.x + mSize.x > cameraPosition.x &&
                    mPosition.y < cameraPosition.y + screenSize.y &&
                    mPosition.y + mSize.y > cameraPosition.y
                );
            }
        protected:
            Vector mPosition;
            Vector mSize;
    };
    class StaticObject : public Object
    {
        public:
            StaticObject(const Vector position, const Vector size) : 
            Object(position, size) {}
            virtual ~StaticObject() = default;
    }; 
    class DynamicObject : public Object
    {   
        public:
            DynamicObject(Vector position,Vector size) : 
            Object(position, size), 
            mVelocity(Vector::zero()), 
            mPassingThroughPlatform(false),
            mAboveCrate(false), 
            mOnGround(false), 
            mFalling(false) {}
            Vector getVelocity() const { return mVelocity; };
            virtual ~DynamicObject() = default;
            virtual void update(float deltaTime) = 0;
            virtual void handleEvent(SDL_Event& e) = 0;
            bool isOnGround() { return mOnGround; };
            bool isFalling()  { return mFalling; };
            bool isPassingThroughPlatform() const { return mPassingThroughPlatform; };
            void setVelocity(Vector velocity) { mVelocity = velocity; };
            void setOnGround(bool onGround) {mOnGround = onGround; };
            void setFalling(bool falling) {mFalling = falling; };
            void setAboveCrate(bool aboveCrate) {mAboveCrate = aboveCrate; };
        protected:
            Vector mVelocity;
            bool mPassingThroughPlatform; 
            bool mAboveCrate; 
            bool mOnGround;   
            bool mFalling;    
    };
}
#endif