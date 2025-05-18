#include "../include/bettlerider/Game.h"
#include "../include/bettlerider/Globals.h"
#include <SDL2/SDL.h>
#include <iostream>

void applyCRTEffect(SDL_Renderer* renderer, SDL_Texture* source);
SDL_Texture* crtEffectsTexture = nullptr;
SDL_Texture* crtRenderTarget = nullptr;
int main( int argc, char *args[] )
{
  //const int TARGET_FPS = 144;
  //const int FRAME_DELAY = 1000 / TARGET_FPS;
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    std::cerr 
    << "SDL could not initialize! SDL_Error: " 
    << SDL_GetError()
    << std::endl;
    return -1;
  }
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode( 0, &displayMode );
  SDL_Window *window = SDL_CreateWindow
  ( 
    "Bettle Rider",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    BRTC::SCREEN_WIDTH,
    BRTC::SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI 
  );
  if( window == nullptr )
    {
      std::cerr 
      << "Window could not be created! SDL_Error: " 
      << SDL_GetError()
      << std::endl;
      SDL_Quit();
    return -1;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer
  (
    window, 
    -1, 
    SDL_RENDERER_ACCELERATED | 
    SDL_RENDERER_TARGETTEXTURE //| 
    //SDL_RENDERER_PRESENTVSYNC
  );
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_RenderSetLogicalSize(renderer, BRTC::SCREEN_WIDTH, BRTC::SCREEN_HEIGHT);
  if( renderer == nullptr )
  {
    std::cerr 
    << "Renderer could not be created! SDL_Error: " 
    << SDL_GetError()
    << std::endl;
    SDL_DestroyWindow( window );
    SDL_Quit();
    return -1;
  }

  crtRenderTarget = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_TARGET,
    BRTC::SCREEN_WIDTH,
    BRTC::SCREEN_HEIGHT
  );

  crtEffectsTexture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_TARGET,
    BRTC::SCREEN_WIDTH,
    BRTC::SCREEN_HEIGHT
  );


  Uint64 lastTime = SDL_GetPerformanceCounter();
  float accumulatedTime = 0.0f;
  BRTC::Game game( window, renderer );
  while( game.isRunning() )
  {
    Uint32 frameStart = SDL_GetTicks();
    Uint64 currentTime = SDL_GetPerformanceCounter();
    BRTC::deltaTime = (float)(currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
    lastTime = currentTime;
    game.handleEvents();
    game.update();

    SDL_SetRenderTarget(renderer, crtRenderTarget);
    SDL_RenderClear(renderer);
    game.render();

    SDL_SetRenderTarget(renderer, crtEffectsTexture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, crtRenderTarget, NULL, NULL);
    applyCRTEffect(renderer, crtRenderTarget);
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, crtEffectsTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    //if (frameTime < FRAME_DELAY) { SDL_Delay(FRAME_DELAY - frameTime); }
  }
  SDL_DestroyTexture(crtEffectsTexture);
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();
  return 0;
}
void applyCRTEffect(SDL_Renderer* renderer, SDL_Texture* source)
{
    int width, height;
    SDL_QueryTexture(source, NULL, NULL, &width, &height);

    // Ajuste estes valores para controlar a intensidade dos efeitos
    const Uint8 scanlineAlpha = 200;  // Mais suave que 100
    const Uint8 vignetteAlpha = 80;  // Mais suave que 80

    // Scanlines mais suaves
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, scanlineAlpha);
    
    for(int y = 0; y < height; y += 2) {
        SDL_Rect scanline = {0, y, width, 1};
        SDL_RenderFillRect(renderer, &scanline);
    }

    // Vinhetagem gradiente (mais suave)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Crie um gradiente para as bordas
    int borderSize = height/6;  // Um pouco maior mas mais suave
    for(int i = 0; i < borderSize; i++) {
        Uint8 alpha = (Uint8)(vignetteAlpha * (1.0 - (float)i/borderSize));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        
        // Topo
        SDL_Rect top = {0, i, width, 1};
        // Base
        SDL_Rect bottom = {0, height - i - 1, width, 1};
        // Esquerda
        SDL_Rect left = {i, 0, 1, height};
        // Direita
        SDL_Rect right = {width - i - 1, 0, 1, height};
        
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }
}