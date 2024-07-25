#ifndef SOLIDPLATFORM_H
#define SOLIDPLATFORM_H

#include "Platform.h"

class SolidPlatform : public Platform
{
public:
    SolidPlatform(int x, int y, int width, int height);

    void render(SDL_Renderer *renderer) override;
};

#endif // SOLIDPLATFORM_H
