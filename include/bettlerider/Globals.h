#ifndef __BETTLE_RIDER_GLOBALS_HEADER_H__
#define __BETTLE_RIDER_GLOBALS_HEADER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <json-c/json.h>
#include <list>
#include <string>


namespace BRTC
{

inline const int SCREEN_WIDTH = 800;
inline const int SCREEN_HEIGHT = 600;
extern SDL_Renderer* renderer = nullptr;


}

#endif // __BETTLE_RIDER_GLOBALS_HEADER_H__