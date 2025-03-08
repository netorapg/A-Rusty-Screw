#ifndef CRATE_H
#define CRATE_H

#include <SDL2/SDL.h>
#include "../Scenario.h"
#include <list>
#include "../platforms/SolidPlatform.h"
#include "../platforms/Platform.h"
#include "../wall/Wall.h"

class SolidPlatform; // Forward declaration
class Wall; // Forward declaration
class Platform; // Forward declaration

class Crate : public Scenario {
public:
    Crate(float x, float y, float width, float height);
    void render(SDL_Renderer* renderer, float cameraX, float cameraY) override; // Adicione os parâmetros da câmera
    void update(const std::list<SolidPlatform>& SolidPlatforms, const std::list<Wall>& Walls, const std::list<Platform>& Platforms); // Atualiza a posição da crate
    bool isVisible(float cameraX, float cameraY, int screenWidth, int screenHeight) override; // Adicionando o método isVisible

private:
    bool isCollidingWithPlatforms(const std::list<SolidPlatform>& SolidPlatforms, const std::list<Platform>& Platforms); // Verifica se a crate está colidindo com plataformas
    bool isCollidingWithWalls(const std::list<Wall>& Walls); // Verifica se a crate está colidindo com paredes
    bool checkCollision(float x1, float y1, float w1, float h1, 
                        float x2, float y2, float w2, float h2) {
        return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
    }
};

#endif // CRATE_H
