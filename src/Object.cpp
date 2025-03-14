#include "../include/bettlerider/Object.h"

namespace BRTC
{

bool Object::isVisible( float cameraX,
                        float cameraY,
                        float screenWidth,
                        float screenHeight )
{
  return ( mX + mWidth > cameraX && mX < cameraX + screenWidth &&
           mY + mHeight > cameraY && mY < cameraY + screenHeight );
}
}   // namespace BRTC
