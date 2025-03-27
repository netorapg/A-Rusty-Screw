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


  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode( 0, &displayMode );

  // Criar a janela
 SDL_Window *window = SDL_CreateWindow( "Bettle Rider",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         displayMode.w,
                                          displayMode.h,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
  if( window == nullptr )
  {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return -1;
  }


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

  const int TARGET_FPS = 60;
  const int FRAME_DELAY = 1000 / TARGET_FPS;

  BRTC::Game game( window, renderer );


  while( game.isRunning() )
  {
    Uint32 frameStart = SDL_GetTicks();
    game.handleEvents();
    game.update();
    game.render();

    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if( frameTime < FRAME_DELAY )
    {
      SDL_Delay( FRAME_DELAY - frameTime );
    }
  }


  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();

  return 0;
}
