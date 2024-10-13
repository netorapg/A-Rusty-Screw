#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include "../Object.h"

class Crate : public Object {
    public:
        Crate(float x, float y, float width, float height);
      
        void render(SDL_Renderer *renderer);
};

#endif // CRATE_H