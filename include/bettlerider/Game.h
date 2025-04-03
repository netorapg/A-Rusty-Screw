#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <json-c/json.h>
#include <list>
#include <string>
#include <iostream>
//#include "config.h"
#include <unordered_map>
#include <json-c/json.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <iostream>


#include "Platform.h"
#include "SolidPlatform.h"
#include "Wall.h"
#include "Crate.h"
#include "Door.h"
#include "Decoration.h"
#include "Player.h"
#include "PhysicsEngine.h"
#include "Camera.h"
#include "Globals.h"


namespace BRTC
{

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
    std::list<Door> mDoors;
    std::list<Decoration> mDecorations;
    std::string mPlatformsTexturePath;
    const float PLAYER_ZOOM_FACTOR = 2.5f;
    float effectiveScreenWidth = SCREEN_WIDTH / PLAYER_ZOOM_FACTOR;
    float effectiveScreenHeight = SCREEN_HEIGHT / PLAYER_ZOOM_FACTOR;
    Player mPlayer;
    Camera mCamera;

    TTF_Font* mFont;
    TTF_Font* mSmallFont;

    Mix_Music* mMusic;
    Mix_Chunk* mJumpSound;

    SDL_Texture* mBackgroundTexture;

    void renderText(const char* text, int x, int y, TTF_Font* font);
    void loadLevelFromJSON(const std::string& filePath);

    Vector mCAmeraPosition;
    Vector mCameraSize;

    float mapWidth;
    float mapHeight;
    bool isTransitioning = false;
    Uint32 transitionStartTime = 0;
    const Uint32 TRANSITION_DELAY = 500;
    const Uint32 HALF_TRANSITION = TRANSITION_DELAY / 2;
    std::string targetLevel;
    Vector targetSpawn;
    int alpha = 0;
    bool increasing = true;
    bool mPlayerActivated;
    Uint32 mActivationTime;
};
}

#endif // GAME_H