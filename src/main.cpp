#include "../include/bettlerider/GameManager.h"
#include "../include/bettlerider/Globals.h"
#include <SDL2/SDL.h>
#include <iostream>

// Funções e variáveis para efeito CRT (mantidas iguais)
void applyCRTEffect(SDL_Renderer* renderer, SDL_Texture* source);
SDL_Texture* crtEffectsTexture = nullptr;
SDL_Texture* crtRenderTarget = nullptr;

int main(int argc, char *args[]) 
{
    // Inicialização do SDL (mantida igual)
    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_Window* window = SDL_CreateWindow(
        "Bettle Rider",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        BRTC::SCREEN_WIDTH,
        BRTC::SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if(window == nullptr) 
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(renderer, BRTC::SCREEN_WIDTH, BRTC::SCREEN_HEIGHT);

    if(renderer == nullptr) 
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Configuração das texturas CRT (mantida igual)
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

    // Criação do GameManager ao invés do Game
    BRTC::GameManager gameManager(window, renderer);

    Uint64 lastTime = SDL_GetPerformanceCounter();
    
    // Loop principal do jogo
    while(gameManager.isRunning()) 
    {
        Uint32 frameStart = SDL_GetTicks();
        Uint64 currentTime = SDL_GetPerformanceCounter();
        
        // Cálculo do deltaTime
        BRTC::deltaTime = (float)(currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
        lastTime = currentTime;

        // Passe o deltaTime para o GameWorld
      gameManager.getGameWorld().setDeltaTime(BRTC::deltaTime);
        // Processamento de eventos
        gameManager.handleEvents();

        // Atualização do jogo
        gameManager.update();

        // Renderização com efeito CRT
        SDL_SetRenderTarget(renderer, crtRenderTarget);
        SDL_RenderClear(renderer);
        gameManager.render();

        // Aplicação do efeito CRT
        SDL_SetRenderTarget(renderer, crtEffectsTexture);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, crtRenderTarget, NULL, NULL);
        applyCRTEffect(renderer, crtRenderTarget);

        // Renderização final
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, crtEffectsTexture, NULL, NULL);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        // Controle de FPS (opcional)
        // if (frameTime < FRAME_DELAY) { SDL_Delay(FRAME_DELAY - frameTime); }
    }

    // Limpeza (mantida igual)
    SDL_DestroyTexture(crtEffectsTexture);
    SDL_DestroyTexture(crtRenderTarget);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Implementação do efeito CRT (mantida igual)
void applyCRTEffect(SDL_Renderer* renderer, SDL_Texture* source)
{
    int width, height;
    SDL_QueryTexture(source, NULL, NULL, &width, &height);

    const Uint8 scanlineAlpha = 200;
    const Uint8 vignetteAlpha = 80;

    // Scanlines
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, scanlineAlpha);
    
    for(int y = 0; y < height; y += 2) {
        SDL_Rect scanline = {0, y, width, 1};
        SDL_RenderFillRect(renderer, &scanline);
    }

    // Vinhetagem
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    int borderSize = height/6;
    for(int i = 0; i < borderSize; i++) {
        Uint8 alpha = (Uint8)(vignetteAlpha * (1.0 - (float)i/borderSize));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        
        SDL_Rect top = {0, i, width, 1};
        SDL_Rect bottom = {0, height - i - 1, width, 1};
        SDL_Rect left = {i, 0, 1, height};
        SDL_Rect right = {width - i - 1, 0, 1, height};
        
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }
}