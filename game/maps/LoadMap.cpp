#include <iostream>
#include <fstream>
#include <vector>
#include "Tile.h"

const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

std::vector<Tile> loadMap(const char* path) {
    std::vector<Tile> tiles;
    std::ifstream map(path);

    if (!map.is_open()) {
        std::cerr << "Failed to load map file: " << path << std::endl;
        return tiles;
    }

    int x = 0, y = 0;
    for (int i = 0; i < TOTAL_TILES; ++i) {
        int tileType;
        map >> tileType;

        if (map.fail()) {
            std::cerr << "Error loading map: unexpected end of file!" << std::endl;
            break;
        }

        if (tileType >= 0 && tileType < TOTAL_TILE_SPRITES) {
            tiles.emplace_back(x, y, tileType);
        }

        x += TILE_WIDTH;
        if (x >= LEVEL_WIDTH) {
            x = 0;
            y += TILE_HEIGHT;
        }
    }

    map.close();
    return tiles;
}