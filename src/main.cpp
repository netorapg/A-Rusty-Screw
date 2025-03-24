#include "../include/bettlerider/Game.h"
#include "../include/bettlerider/Globals.h"

#include <SDL2/SDL.h>
#include <iostream>

int main( int argc, char *args[] )
{
  // Inicializar SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return -1;
  }

  // Criar a janela
  SDL_Window *window = SDL_CreateWindow( "Platform 2D",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         BRTC::SCREEN_WIDTH,
                                         BRTC::SCREEN_HEIGHT,
                                         SDL_WINDOW_SHOWN );
  if( window == nullptr )
  {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return -1;
  }

  // Criar o renderizador
  SDL_Renderer *renderer = SDL_CreateRenderer(
    window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  if( renderer == nullptr )
  {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow( window );
    SDL_Quit();
    return -1;
  }

  // Criar o jogo
  BRTC::Game game( window, renderer );

  // Loop principal do jogo
  while( game.isRunning() )
  {
    game.handleEvents();
    game.update();
    game.render();
  }

  // Limpar recursos
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();

  return 0;
}
