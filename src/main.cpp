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
                                         BRTC::SCREEN_WIDTH,
                                          BRTC::SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI );
  if( window == nullptr )
  {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return -1;
  }


  SDL_Renderer *renderer = SDL_CreateRenderer(
    window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, BRTC::SCREEN_WIDTH, BRTC::SCREEN_HEIGHT);
  if( renderer == nullptr )
  {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow( window );
    SDL_Quit();
    return -1;
  }

Uint64 lastTime = SDL_GetPerformanceCounter();
float accumulatedTime = 0.0f;

  BRTC::Game game( window, renderer );


  while( game.isRunning() )
  {
    SDL_Log("Rodando loop do jogo...");
    Uint64 currentTime = SDL_GetPerformanceCounter();
    BRTC::deltaTime = (float)(currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
    lastTime = currentTime;
    std::cout << "[Frame Start] DeltaTime: " << BRTC::deltaTime << "s" << std::endl;

   game.handleEvents();

   
     game.update();
 
   game.render();


    
std::cout << "DeltaTime: " << BRTC::deltaTime << "s | FPS: " << 1.0f / BRTC::deltaTime << std::endl;
    
  }


  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();

  return 0;
}
