#pragma once
#include <SDL3/SDL.h>
namespace V {
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_IOS)
constexpr bool IS_MOBILE = true;
#else
constexpr bool IS_MOBILE = false;
#endif
constexpr int RENDER_WIDTH = 640;
constexpr int RENDER_HEIGHT = 480;
constexpr float WINDOW_SCALE = 2.0;
constexpr const char* GAME_AUTHOR = "ws3917";
constexpr const char* GAME_NAME = "Project Spacetime 0";
constexpr const char* GAME_SHORTNAME = "spacetime0";
}  // namespace V