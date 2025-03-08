#include "CollisionManager.h"

void CollisionManager::handlePlatformCollision(
  float &posY,
  float &velY,
  bool &onGround,
  bool &falling,
  bool passingThroughPlatform,
  std::list<Platform> &platforms,
  std::list<SolidPlatform> &solidPlatforms)
{
    // Colisão com plataformas sólidas
    for (const auto &platform : solidPlatforms)
    {
        if (checkCollision(0, posY, playerWidth, 26, platform.getX(), platform.getY(),
                           platform.getWidth(), platform.getHeight()))
        {
            if (velY > 0)  // Colisão com a parte inferior (plataforma)
            {
                posY = platform.getY() - 26;
                velY = 0;
                onGround = true;
                falling = false;
            }
            else if (velY < 0)  // Colisão com a parte superior (plataforma)
            {
                posY = platform.getY() + platform.getHeight();
                velY = 0;
            }
        }
    }

    // Colisão com plataformas não sólidas (passando por elas)
    for (const auto &platform : platforms)
    {
        if (!passingThroughPlatform &&
            checkCollision(0, posY + playerWidth, 26, 1, platform.getX(), platform.getY(),
                           platform.getWidth(), platform.getHeight()) &&
            velY >= 0)
        {
            posY = platform.getY() - 26;
            velY = 0;
            onGround = true;
            falling = false;
            break;
        }
    }

    if (!onGround)
        falling = true;  // Se não está no chão, está caindo
}

void CollisionManager::handleWallCollision(float &posX, float &velX, std::list<Wall> &walls)
{
    for (const auto &wall : walls)
    {
        if (checkCollision(posX, 0, width, 1, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
        {
            if (velX > 0)  // Colisão pela direita
                posX = wall.getX() - width;
            else if (velX < 0)  // Colisão pela esquerda
                posX = wall.getX() + wall.getWidth();
            velX = 0;
        }
    }
}

void CollisionManager::handleCrateCollision(
  float &posX,
  float &posY,
  float &velX,
  float &velY,
  bool &aboveCrate,
  bool &onGround,
  bool &falling,
  std::list<Crate> &crates)
{
    for (auto &crate : crates)
    {
        if (checkCollision(posX, posY, 25, 26, crate.getX(), crate.getY(), crate.getWidth(), crate.getHeight()))
        {
            if (velX > 0 && !aboveCrate)
            {
                crate.setX(crate.getX() + velX);
                posX = crate.getX() - 25;
            }
            else if (velX < 0 && !aboveCrate)
            {
                crate.setX(crate.getX() + velX);
                posX = crate.getX() + crate.getWidth();
            }
            else if (velY > 0)  // Colisão por cima (caindo)
            {
                posY = crate.getY() - 26;
                aboveCrate = true;
                falling = false;
                onGround = true;
                velY = 0;
            }
        }
        else if (aboveCrate)  // Se o jogador está acima do crate e a colisão não é mais válida
        {
            if (posY + 26 < crate.getY() || posY > crate.getY() + crate.getHeight())
                aboveCrate = false;
        }
    }
}
