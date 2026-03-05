#include "game.hpp"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <physfs.h>

#include <stdexcept>

#include "values.hpp"

Game::Game() {
  window.reset(SDL_CreateWindow(
      V::GAME_NAME, static_cast<int>(V::WINDOW_SCALE * V::RENDER_WIDTH),
      static_cast<int>(V::WINDOW_SCALE * V::RENDER_HEIGHT),
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE));
  renderer.reset(SDL_CreateRenderer(window.get(), nullptr));
  SDL_SetRenderVSync(renderer.get(), 1);
  SDL_SetDefaultTextureScaleMode(renderer.get(), SDL_SCALEMODE_PIXELART);
  SDL_SetRenderLogicalPresentation(renderer.get(), V::RENDER_WIDTH,
                                   V::RENDER_HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  mixer.reset(MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 0));
}
void Game::run() {
  const float TICK_FREQ =
      static_cast<float>(SDL_GetPerformanceFrequency()) / 1000;
  uint64_t current_tick;
  SDL_Event event;
  last_tick = SDL_GetPerformanceCounter();
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) running = false;
      SDL_ConvertEventToRenderCoordinates(renderer.get(), &event);
      // input(event)
    }
    current_tick = SDL_GetPerformanceCounter();
    float dt = (current_tick - last_tick) / TICK_FREQ;
    last_tick = current_tick;
    // update(delta)
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());
    // draw(renderer)
    SDL_RenderPresent(renderer.get());
  }
}
GameContext::GameContext() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
  if (!MIX_Init()) {
    SDL_Quit();
    throw std::runtime_error(std::string("MIX_Init failed: ") + SDL_GetError());
  }
  try {
    bool data_mount_success = false;
    bool save_mount_success = false;

#ifdef SDL_PLATFORM_ANDROID
    // 修复 Android 内存泄漏：使用栈变量而不是 new
    PHYSFS_AndroidInit android_init = {SDL_GetAndroidJNIEnv(),
                                       SDL_GetAndroidActivity()};
    if (!PHYSFS_init(reinterpret_cast<const char*>(&android_init)))
      throw std::runtime_error(PHYSFS_getLastError());

    data_mount_success = PHYSFS_mount(PHYSFS_getBaseDir(), "/", 0);
    const char* ext_path = SDL_GetAndroidExternalStoragePath();
    if (ext_path) {
      save_mount_success = PHYSFS_mount(ext_path, "/user", 1);
      PHYSFS_setWriteDir(ext_path);
    }
#else
    const char* base_path = SDL_GetBasePath();
    if (!base_path) throw std::runtime_error("SDL_GetBasePath failed");

    if (!PHYSFS_init(base_path)) {
      throw std::runtime_error(PHYSFS_getLastError());
    }

    char data_path[1024];
    SDL_snprintf(data_path, sizeof(data_path), "%smain.pak", base_path);
    data_mount_success = PHYSFS_mount(data_path, "/", 0);

    char* pref_path = SDL_GetPrefPath(V::GAME_AUTHOR, V::GAME_SHORTNAME);
    if (pref_path) {
      save_mount_success = PHYSFS_mount(pref_path, "/user", 1);
      PHYSFS_setWriteDir(pref_path);
      SDL_free(pref_path);
    }
#endif
    if (!data_mount_success || !save_mount_success) {
      throw std::runtime_error("Mount data / save failed");
    }
  } catch (std::exception& e) {
    MIX_Quit();
    SDL_Quit();
    throw;
  }
}

void Game::AssetDeleter::operator()(SDL_Window* window) const {
  if (window) SDL_DestroyWindow(window);
}
void Game::AssetDeleter::operator()(SDL_Renderer* renderer) const {
  if (renderer) SDL_DestroyRenderer(renderer);
}
void Game::AssetDeleter::operator()(MIX_Mixer* mixer) const {
  if (mixer) MIX_DestroyMixer(mixer);
}
GameContext::~GameContext() {
  PHYSFS_deinit();
  MIX_Quit();
  SDL_Quit();
}