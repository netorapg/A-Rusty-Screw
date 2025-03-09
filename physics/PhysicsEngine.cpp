#include "PhysicsEngine.h"

#include <iostream>

namespace
{
bool CheckCollision( float x1,
                     float y1,
                     float w1,
                     float h1,
                     float x2,
                     float y2,
                     float w2,
                     float h2 )
{
  return ( x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2 );
}
}   // namespace

void PhysicsEngine::HandleCollisions(
  Entity                         &entity,
  const std::list<Platform>      &platforms,
  const std::list<SolidPlatform> &solidPlatforms,
  const std::list<Wall>          &walls,
  std::list<Crate>               &crates )
{
  // Resetar estados
  entity.setOnGround( false );
  entity.setAboveCrate( false );

  // Obter dados do jogador
  float       px = entity.getPosX();
  float       py = entity.getPosY();
  const float pw = entity.getWidth();
  const float ph = entity.getHeight();
  float       vx = entity.getHorizontalVelocity();
  float       vy = entity.getVerticalVelocity();

  // Colisão com paredes
  for( const auto &wall : walls )
  {
    if( CheckCollision( px,
                        py,
                        pw,
                        ph,
                        wall.getX(),
                        wall.getY(),
                        wall.getWidth(),
                        wall.getHeight() ) )
    {
      // Resolver horizontal
      if( vx > 0 )
        px = wall.getX() - pw;
      else if( vx < 0 )
        px = wall.getX() + wall.getWidth();
      entity.setHorizontalVelocity( 0 );
    }
  }

    for( auto &crate : crates )
    {
      // Colisão entre jogador e caixa
      if( crate.checkCollision( entity.getPosX(),
                                entity.getPosY(),
                                entity.getWidth(),
                                entity.getHeight() ) )
      {
        // Empurrar caixa horizontalmente
        if( entity.getHorizontalVelocity() != 0 )
          crate.applyForce( entity.getHorizontalVelocity() * 0.8f, 0 );

        // Colisão vertical
        if( entity.getVerticalVelocity() > 0 )
        {   // Jogador caindo
          entity.setOnGround( true );
          entity.setVerticalVelocity( 0 );
          entity.setPosition( entity.getPosX(),
                              crate.getY() - entity.getHeight() );
        }
      }
    }

  // Colisão com plataformas sólidas
  for( const auto &platform : solidPlatforms )
  {
    if( CheckCollision( px,
                        py,
                        pw,
                        ph,
                        platform.getX(),
                        platform.getY(),
                        platform.getWidth(),
                        platform.getHeight() ) )
    {
      // Resolver vertical
      if( vy > 0 )
      {   // Caindo
        py = platform.getY() - ph;
        entity.setOnGround( true );
        vy = 0;
      }
      else if( vy < 0 )
      {   // Subindo
        py = platform.getY() + platform.getHeight();
        vy = 0;
      }
    }
  }

  // Colisão com plataformas normais
  if( !entity.isPassingThroughPlatform() )
  {
    for( const auto &platform : platforms )
    {
      if( CheckCollision( px,
                          py + ph - 1,
                          pw,
                          1,   // Apenas pés
                          platform.getX(),
                          platform.getY(),
                          platform.getWidth(),
                          platform.getHeight() ) )
      {
        py = platform.getY() - ph;
        entity.setOnGround( true );
        vy = 0;
      }
    }
  }

  // Aplicar mudanças
  entity.setPosition( px, py );
  entity.setVerticalVelocity( vy );
  entity.setHorizontalVelocity( vx );
}
