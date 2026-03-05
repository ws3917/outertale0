#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <physfs.h>

#include <stdexcept>

#include "gamecontext.hpp"
#include "values.hpp"

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
GameContext::~GameContext() {
  PHYSFS_deinit();
  MIX_Quit();
  SDL_Quit();
}