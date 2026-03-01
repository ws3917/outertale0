#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <physfs.h>

#include "save.hpp"
#include "utils.hpp"
#include "values.hpp"

int main(int, char** argv) {
  // 读取一张图片并显示
  if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
  if (MIX_Init()) return 1;
  bool mount_success = false;
#ifdef SDL_PLATFORM_ANDROID
  PHYSFS_init(reinterpret_cast<char*>(new PHYSFS_AndroidInit{
      SDL_GetAndroidJNIEnv(), SDL_GetAndroidActivity()}));
  mount_success = PHYSFS_mount(PHYSFS_getBaseDir(), "/", 0);
#else
  PHYSFS_init(argv[0]);
  char data_path[1024];
  SDL_snprintf(data_path, sizeof(data_path), "%smain.pak", PHYSFS_getBaseDir());
  mount_success = PHYSFS_mount(data_path, "/assets", 0);
#endif
  if (!mount_success) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't mount game data! ");
    return 1;
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Mounted game data");
  Save save;
  SDL_CreateWindowAndRenderer(
      "hello", static_cast<int>(V::WINDOW_SCALE * V::RENDER_WIDTH),
      static_cast<int>(V::WINDOW_SCALE * V::RENDER_HEIGHT),
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &save.meta.window,
      &save.meta.renderer);
  SDL_SetRenderVSync(save.meta.renderer, 1);
  SDL_SetRenderLogicalPresentation(save.meta.renderer, V::RENDER_WIDTH,
                                   V::RENDER_HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  SDL_Event event;
  SDL_Surface* img = SDL_LoadPNG_IO(U::IOFromFile("image/bg_static.png"), true);
  SDL_Texture* tex = SDL_CreateTextureFromSurface(save.meta.renderer, img);
  SDL_DestroySurface(img);
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) running = false;
      SDL_ConvertEventToRenderCoordinates(save.meta.renderer, &event);
    }
    SDL_RenderClear(save.meta.renderer);
    SDL_RenderTexture(save.meta.renderer, tex, nullptr, nullptr);
    SDL_RenderPresent(save.meta.renderer);
  }
  PHYSFS_deinit();
  MIX_Quit();
  SDL_Quit();
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Game stopped successfully!");
  return 0;
}