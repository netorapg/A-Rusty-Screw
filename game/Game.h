#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <json-c/json.h>
#include <list>
#include <string>

#include "../player/Player.h"
#include "../platforms/Platform.h"
#include "../platforms/SolidPlatform.h"
#include "../wall/Wall.h"
#include "../crate/Crate.h"
#include "../physics/PhysicsEngine.h"

class Game
{
public:
    Game(SDL_Window* window, SDL_Renderer* renderer);
    ~Game();

    void handleEvents();
    void update();
    void render();
    bool isRunning();
    void resetGame();

private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    bool mQuit;

    std::list<Platform> mPlatforms;
    std::list<SolidPlatform> mSolidPlatforms;
    std::list<Wall> mWalls;
    std::list<Crate> mCrates;
    Player mPlayer;
    SDL_Rect mCamera;

    TTF_Font* mFont;
    TTF_Font* mSmallFont;

    Mix_Music* mMusic;
    Mix_Chunk* mJumpSound;

    SDL_Texture* mBackgroundTexture;

    void renderText(const char* text, int x, int y, TTF_Font* font);
    void loadLevelFromJSON(const std::string& filePath);
};

#endif // GAME_H