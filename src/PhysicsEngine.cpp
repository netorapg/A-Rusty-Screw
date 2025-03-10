#include "PhysicsEngine.h"

#include <iostream>

namespace
{
bool CheckCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}
} // namespace

void PhysicsEngine::HandleCollisions(
  Entity &entity,
  const std::list<Platform> &platforms,
  const std::list<SolidPlatform> &solidPlatforms,
  const std::list<Wall> &walls,
  std::list<Crate> &crates,
  const std::list<Door> &doors, std::string &levelToLoad)
{
  // Resetar estados
  entity.setOnGround(false);
  entity.setAboveCrate(false);

  // Obter dados do jogador
  float px = entity.getPosX();
  float py = entity.getPosY();
  const float pw = entity.getWidth();
  const float ph = entity.getHeight();
  float vx = entity.getHorizontalVelocity();
  float vy = entity.getVerticalVelocity();
  bool onPlatform = false; // Verifica se o jogador está em cima de uma plataforma

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
      entity.setHorizontalVelocity(0);
    }
  }

  // Colisão com caixas
  for (auto &crate : crates)
  {
    if (crate.checkCollision(entity.getPosX(), entity.getPosY(), entity.getWidth(), entity.getHeight()))
    {
      // Empurrar caixa horizontalmente
      if (entity.getHorizontalVelocity() != 0)
        crate.applyForce(entity.getHorizontalVelocity() * 0.8f, 0);

      // Colisão vertical
      if (entity.getVerticalVelocity() > 0)
      { // Jogador caindo
        entity.setOnGround(true);
        entity.setFalling(false); // Jogador não está mais caindo
        entity.setVerticalVelocity(0);
        entity.setPosition(entity.getPosX(), crate.getY() - entity.getHeight());
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
            entity.setOnGround(true);
            entity.setFalling(false);
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
  if (!entity.isPassingThroughPlatform())
  {
    for (const auto &platform : platforms)
    {
      if (CheckCollision(px, py + ph - 1, pw, 1, platform.getX(), platform.getY(), platform.getWidth(), platform.getHeight()))
      {
        py = platform.getY() - ph;
        entity.setOnGround(true);
        entity.setFalling(false); // Jogador não está mais caindo
        vy = 0;
        onPlatform = true;
        break; // Sai do loop, pois já encontrou uma plataforma
      }
    }

    // Se não estiver em cima de nenhuma plataforma, marque como "não no chão"
    if (!onPlatform && entity.isOnGround())
    {
      entity.setOnGround(false);
      entity.setFalling(true); // Jogador está caindo
    }
  }



  // Aplicar mudanças
  entity.setPosition(px, py);
  entity.setVerticalVelocity(vy);
  entity.setHorizontalVelocity(vx);

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
