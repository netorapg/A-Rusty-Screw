#include "../include/arscrew/GameManager.h"
#include "../include/arscrew/Globals.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include <cstring>

void applyCRTEffect(SDL_Renderer* renderer);

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: "
                  << SDL_GetError()
                  << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: "
                  << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Bettle Rider",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        ARSCREW::SCREEN_WIDTH,
        ARSCREW::SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC 
    );
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    

    // Garantir que o "clear" apague sempre com preto 100% opaco
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Ajustes de escala (caso use LogicalSize)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(renderer, ARSCREW::SCREEN_WIDTH, ARSCREW::SCREEN_HEIGHT);

    ARSCREW::GameManager gameManager(window, renderer);
    Uint64 lastTime = SDL_GetPerformanceCounter();

    while (gameManager.isRunning()) {
        // Calcula deltaTime
        Uint64 currentTime = SDL_GetPerformanceCounter();
        ARSCREW::deltaTime = float(currentTime - lastTime) /
                          float(SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        gameManager.handleEvents();
        gameManager.update(ARSCREW::deltaTime);

        // ---------- 1) LIMPAR A TELA (janela) (preto opaco) ----------
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // ---------- 2) RENDERIZAR O JOGO DIRETO NA JANELA ----------
        gameManager.render();

        // ---------- 3) APLICAR CRT (scanlines + vinheta) DIRECT NA JANELA ----------
        applyCRTEffect(renderer);

        // ---------- 4) APRESENTAR (com VSYNC) ----------
        SDL_RenderPresent(renderer);

        // OBS: não precisa de SDL_Delay, pois PRESENTVSYNC já "espera" o próximo VBlank.
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void applyCRTEffect(SDL_Renderer* renderer)
{
    // Para obter w/h, podemos usar Globals ou
    int width  = ARSCREW::SCREEN_WIDTH;
    int height = ARSCREW::SCREEN_HEIGHT;

    // Novos valores MUITO mais suaves:
    const Uint8 scanlineAlpha = 15;   // quase transparente
    const Uint8 vignetteAlpha = 30;   // vinheta leve

    // Precisamos de blend apenas para o CRT, não para o game
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 1) Scanlines — cada linha par ganha um retângulo preto semitransparente
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, scanlineAlpha);
    for (int y = 0; y < height; y += 2) {
        SDL_Rect scanline = { 0, y, width, 1 };
        SDL_RenderFillRect(renderer, &scanline);
    }

    // 2) Vinheta gradiente — nas bordas, desenha retângulos cada vez mais transparentes
    int borderSize = height / 6;
    for (int i = 0; i < borderSize; i++) {
        // a cada "i" aproximamos 1 pixel da borda, alfa cai linearmente
        Uint8 alpha = static_cast<Uint8>(vignetteAlpha * (1.0f - float(i) / borderSize));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);

        // Topo e Base
        SDL_Rect top    = { 0, i,         width, 1 };
        SDL_Rect bottom = { 0, height - i - 1, width, 1 };
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);

        // Esquerda e Direita
        SDL_Rect left  = { i, 0, 1, height };
        SDL_Rect right = { width - i - 1, 0, 1, height };
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }

    // Volta ao blend NONE, caso o game queira desenhar algo opaco depois
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
