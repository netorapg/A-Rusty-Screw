#include "../include/bettlerider/PhysicsEngine.h"

#include <iostream>

namespace BRTC
{
  bool CheckCollision(const Object &obj1, const Object &obj2) {
    return (obj1.getX() < obj2.getX() + obj2.getWidth() &&
            obj1.getX() + obj1.getWidth() > obj2.getX() &&
            obj1.getY() < obj2.getY() + obj2.getHeight() &&
            obj1.getY() + obj1.getHeight() > obj2.getY());
  }
  

// PhysicsEngine.cpp
void PhysicsEngine::HandleCollisions(
  DynamicObject& dynamicObject,
  const std::list<Wall>& walls,
  const std::list<Platform>& platforms,
  const std::list<SolidPlatform>& solidPlatforms
) {
  float px = dynamicObject.getX();
  float py = dynamicObject.getY();
  const float pw = dynamicObject.getWidth();
  const float ph = dynamicObject.getHeight();
  float vx = dynamicObject.getHorizontalVelocity();
  float vy = dynamicObject.getVerticalVelocity();

  // Colisão com paredes
  for (const auto& wall : walls) {
      if (CheckCollision(dynamicObject, wall)) {
          if (vx > 0) px = wall.getX() - pw;
          else if (vx < 0) px = wall.getX() + wall.getWidth();
          dynamicObject.setHorizontalVelocity(0);
      }
  }

  // Colisão com plataformas sólidas
  for (const auto& platform : solidPlatforms) {
      if (CheckCollision(dynamicObject, platform)) {
          if (vy > 0) {
              py = platform.getY() - ph;
              vy = 0;
              dynamicObject.setOnGround(true);
          } else if (vy < 0) {
              py = platform.getY() + platform.getHeight();
              vy = 0;
          }
      }
  }

  // Colisão com plataformas normais
  if (!dynamicObject.isPassingThroughPlatform()) {
      for (const auto& platform : platforms) {
          if (CheckCollision(dynamicObject, platform)) {
              if (vy > 0) {
                  py = platform.getY() - ph;
                  vy = 0;
                  dynamicObject.setOnGround(true);
              } else if (vy < 0) {
                  py = platform.getY() + platform.getHeight();
                  vy = 0;
              }
          }
      }
  }

  // Aplicar mudanças
  dynamicObject.setPosition(px, py);
  dynamicObject.setVerticalVelocity(vy);
  dynamicObject.setHorizontalVelocity(vx);
};

void PhysicsEngine::HandlePlayerCollisions(
  Player& player,
  std::list<Crate>& crates,
  const std::list<Door>& doors,
  std::string& levelToLoad
) {
  bool wasOnGround = player.isOnGround();
  bool standingOnCrate = false;

  for (auto& crate : crates) {
      if (CheckCollision(player, crate)) {
          float playerBottom = player.getY() + player.getHeight();
          float crateTop = crate.getY();
          float overlapVertical = playerBottom - crateTop;
          
          float playerRight = player.getX() + player.getWidth();
          float crateLeft = crate.getX();
          float playerLeft = player.getX();
          float crateRight = crate.getX() + crate.getWidth();
          float overlapHorizontal = std::min(playerRight - crateLeft, crateRight - playerLeft);

          // Colisão vertical (jogador em cima/embaixo)
          if (overlapVertical < overlapHorizontal) {
              if (player.getVerticalVelocity() > 0) { // Caindo
                  player.setPosition(player.getX(), crate.getY() - player.getHeight());
                  player.setVerticalVelocity(0);
                  standingOnCrate = true;
                  wasOnGround = true;
              }
          }
          // Colisão horizontal (empurrar)
          else {
              // Empurra a caixa
              float pushForce = player.getHorizontalVelocity() * 1.2f;
              crate.applyForce(pushForce, 0);
              
              // Impede que o jogador atravesse a caixa
              if (player.getHorizontalVelocity() > 0) { // Movendo para direita
                  player.setPosition(crate.getX() - player.getWidth(), player.getY());
              }
              else if (player.getHorizontalVelocity() < 0) { // Movendo para esquerda
                  player.setPosition(crate.getX() + crate.getWidth(), player.getY());
              }
          }
      }
  }

  player.setOnGround(wasOnGround || standingOnCrate);

  // Colisão com portas
  for (const auto& door : doors) {
      if (CheckCollision(player, door)) {
          levelToLoad = door.getLevelToLoad();
          break;
      }
  }
};


}