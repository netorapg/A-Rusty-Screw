#include <SDL2/SDL.h>
#include "game/Game.h"

int main(int argc, char *args[])
{
    Game game;
    if (!game.init())
    {
        printf("Failed to initialize!\n");
        return -1;
    }

    while (!game.isQuit())
    {
        game.handleEvents();
        game.update();
        game.render();
    }

    game.close();

    return 0;
}