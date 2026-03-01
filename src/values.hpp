#include <SDL3/SDL.h>
namespace V {
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_IOS)
inline constexpr bool IS_MOBILE = true;
#else
inline constexpr bool IS_MOBILE = false;
#endif
inline constexpr int RENDER_WIDTH = 640;
inline constexpr int RENDER_HEIGHT = 480;
inline constexpr float WINDOW_SCALE = 2.0;
}  // namespace ws