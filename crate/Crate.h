// Crate.h
#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include "../Object.h"
#include <list>

class SolidPlatform; // Forward declaration

class Crate : public Object {
public:
    Crate(float x, float y, float width, float height);
    void render(SDL_Renderer* renderer, float cameraX, float cameraY); // Adicione os parâmetros da câmera
    void update(const std::list<SolidPlatform>& SolidPlatforms); // Atualiza a posição da crate

private:
    bool isCollidingWithPlatforms(const std::list<SolidPlatform>& SolidPlatforms);
    bool checkCollision(float x1, float y1, float w1, float h1, 
                        float x2, float y2, float w2, float h2) {
        return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
    }
};

#endif // CRATE_H