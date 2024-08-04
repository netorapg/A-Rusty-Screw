#include "game/Game.h"
#include <iostream>

int main(int argc, char* args[]) {
    Game game;

    if (!game.init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    while (!game.isQuit()) {
        game.handleEvents();
        game.update();
        game.render();
    }

    game.close();

    return 0;
}
