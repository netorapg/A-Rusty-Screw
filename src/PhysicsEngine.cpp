#include "../include/bettlerider/PhysicsEngine.h"

#include <iostream>

namespace BRTC
{
bool CheckCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}


void PhysicsEngine::HandleCollisions(
  DynamicObject &DynamicObject,
  const std::list<Platform> &platforms,
  const std::list<SolidPlatform> &solidPlatforms,
  const std::list<Wall> &walls,
  std::list<Crate> &crates,
  const std::list<Door> &doors, std::string &levelToLoad)
{
  // Resetar estados
  DynamicObject.setOnGround(false);
  DynamicObject.setAboveCrate(false);


  float px = DynamicObject.getPosX();
  float py = DynamicObject.getPosY();
  const float pw = DynamicObject.getWidth();
  const float ph = DynamicObject.getHeight();
  float vx = DynamicObject.getHorizontalVelocity();
  float vy = DynamicObject.getVerticalVelocity();
  bool onPlatform = false;

  // Colisão com paredes
  for (const auto &wall : walls)
  {
    if (CheckCollision(px, py, pw, ph, wall.getX(), wall.getY(), wall.getWidth(), wall.getHeight()))
    {
      // Resolver horizontal
      if (vx > 0)
        px = wall.getX() - pw;
      else if (vx < 0)
        px = wall.getX() + wall.getWidth();
      DynamicObject.setHorizontalVelocity(0);
    }
  }

  // Colisão com caixas
  for (auto &crate : crates)
  {
    if (crate.checkCollision(DynamicObject.getPosX(), DynamicObject.getPosY(), DynamicObject.getWidth(), DynamicObject.getHeight()))
    {
      // Empurrar caixa horizontalmente
      if (DynamicObject.getHorizontalVelocity() != 0)
        crate.applyForce(DynamicObject.getHorizontalVelocity() * 0.8f, 0);

      // Colisão vertical
      if (DynamicObject.getVerticalVelocity() > 0)
      { // Jogador caindo
        DynamicObject.setOnGround(true);
        DynamicObject.setFalling(false); // Jogador não está mais caindo
        DynamicObject.setVerticalVelocity(0);
        DynamicObject.setPosition(DynamicObject.getPosX(), crate.getY() - DynamicObject.getHeight());
      }
    }
  }

  // Colisão com plataformas sólidas
  for (const auto &platform : solidPlatforms)
  {
    // Verifique apenas a área dos pés do jogador (1 pixel de altura)
    if (CheckCollision(px, py + ph - 1, pw, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
    {
      if (vy > 0)
        {
            py = platform.getY() - ph; // Posiciona em cima da plataforma
            vy = 0;
            DynamicObject.setOnGround(true);
            DynamicObject.setFalling(false);
            onPlatform = true;
        }
        // Colisão enquanto subindo (velocidade negativa)
        else if (vy < 0)
        {
            py = platform.getY() + platform.getHeight(); // Posiciona abaixo da plataforma
            vy = 0;
        }
      break; // Sai do loop, pois já encontrou uma plataforma
    }

  }

  // Colisão com plataformas normais
  if (!DynamicObject.isPassingThroughPlatform())
  {
    for (const auto &platform : platforms)
    {
      if (CheckCollision(px, py + ph - 1, pw, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
      {
        py = platform.getY() - ph;
        DynamicObject.setOnGround(true);
        DynamicObject.setFalling(false); // Jogador não está mais caindo
        vy = 0;
        onPlatform = true;
        break; // Sai do loop, pois já encontrou uma plataforma
      }
    }

    // Se não estiver em cima de nenhuma plataforma, marque como "não no chão"
    if (!onPlatform && DynamicObject.isOnGround())
    {
      DynamicObject.setOnGround(false);
      DynamicObject.setFalling(true); // Jogador está caindo
    }
  }



  // Aplicar mudanças
  DynamicObject.setPosition(px, py);
  DynamicObject.setVerticalVelocity(vy);
  DynamicObject.setHorizontalVelocity(vx);

  for (const auto &door : doors)
  {
      if (CheckCollision(px, py, pw, ph, door.getX(), door.getY(), door.getWidth(), door.getHeight()))
      {
          std::cout << "Colisão com a porta detectada (usando px, py)!" << std::endl;
          levelToLoad = door.getLevelToLoad();
          break;
      }
  }
  

}

}