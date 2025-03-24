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
inline SDL_Renderer* renderer;

inline constexpr float GRAVITY = 0.5f; 
inline constexpr float FRICTION = 0.85f;
inline constexpr float JUMP_FORCE = -10.0f;
inline constexpr float MOVE_SPEED = 5.0f;

inline const int SCREEN_WIDTH = 1024;
inline const int SCREEN_HEIGHT = 680;


}

#endif // __BETTLE_RIDER_GLOBALS_HEADER_H__