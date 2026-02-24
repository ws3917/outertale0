#include "engine/Global.hpp"
#include "scene/S_Init.hpp"

#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <miniaudio.h>
#include <physfs.h>

#include "engine/AudioMgr.hpp"
#include "engine/FontMgr.hpp"
#include "engine/ImageMgr.hpp"
#include "engine/SceneMgr.hpp"
#include "engine/Utils.hpp"

struct AppState {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  ma_engine* mixer = nullptr;
  uint64_t last_tick = 0;
  Scene* current_scene = nullptr;
};
SDL_AppResult SDL_AppInit(void** _state, int, char** argv) {
  AppState* s = new AppState;
  *_state = s;
  // 初始化
  SDL_Log("--- Game Start ---\n");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("[C] <Init> SDL init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("[I] <Init> SDL core initialized.");

  // 打包资源
  if (!Utils::isMobile()) {
    const char* base_path = SDL_GetBasePath();
    std::string archive_path = std::string(base_path) + "main.pak";
    SDL_free(const_cast<char*>(base_path));
    if (!PHYSFS_init(argv[0]) || !PHYSFS_mount(archive_path.c_str(), "/", 1)) {
      SDL_Log("[C] <Init> Failed to mount physFS: %s",
              PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
      return SDL_APP_FAILURE;
    }
  }

  yyjson_doc* info = Utils::LoadJson("config/info.json");
  if (!info) {
    SDL_Log("[C] <Init> Can't load 'info.json'.");
    return SDL_APP_FAILURE;
  }
  yyjson_val* metadata = yyjson_doc_ptr_get(info, "/metadata");
  const int WIDTH = yyjson_get_int(yyjson_doc_ptr_get(info, "/metadata/width"));
  const int HEIGHT =
      yyjson_get_int(yyjson_doc_ptr_get(info, "/metadata/height"));
  const std::string AUTHOR =
      yyjson_get_str(yyjson_doc_ptr_get(info, "/metadata/author"));
  const std::string NAME =
      yyjson_get_str(yyjson_doc_ptr_get(info, "/metadata/name"));
  if (!WIDTH || !HEIGHT || AUTHOR.empty() || NAME.empty()) {
    SDL_Log("[C] <Init> Some metadata is missing in 'info.json'.");
    yyjson_doc_free(info);
    return SDL_APP_FAILURE;
  }
  yyjson_doc_free(info);
  // 加载配置
  char* pref_path = SDL_GetPrefPath(AUTHOR.c_str(), NAME.c_str());
  std::string user_path = std::string(pref_path) + "savefile.json";
  SDL_free(pref_path);
  yyjson_doc* default_savefile =
      Utils::LoadJson("config/default_savefile.json");
  if (!default_savefile) {
    SDL_Log("[C] <Init> Can't load 'default_savefile.json'.");
    return SDL_APP_FAILURE;
  }
  yyjson_doc* user_savefile = Utils::LoadJson(user_path);
  if (!user_savefile) {
    SDL_Log(
        "[W] <Init> User save data isn't found, will copy the default config.");
    Utils::SaveFile("config/default_savefile.json", user_path);
    user_savefile = Utils::LoadJson(user_path);
  }
  yyjson_doc_free(default_savefile);
  s->mixer = new ma_engine;
  if (ma_engine_init(nullptr, s->mixer) != MA_SUCCESS) {
    SDL_Log("[C] <Init> Mixer device creation failed.");
    return SDL_APP_FAILURE;
  }
  SDL_Log("[I] <Init> Mixer device created.");
  int window_flag = SDL_WINDOW_HIGH_PIXEL_DENSITY;

  if (Utils::isMobile())
    window_flag |= SDL_WINDOW_FULLSCREEN;
  else
    window_flag |= SDL_WINDOW_RESIZABLE;
  s->window = SDL_CreateWindow(NAME.c_str(), 1280, 960, window_flag);
  s->renderer = SDL_CreateRenderer(s->window, nullptr);
  if (!s->window || !s->renderer) {
    SDL_Log("[C] <Init> Window/Renderer creation failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("[I] <Init> Window created: %dx%d.", WIDTH, HEIGHT);
  SDL_SetRenderVSync(s->renderer, 1);
  SDL_SetRenderLogicalPresentation(s->renderer, WIDTH, HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  SDL_Log("[I] <Init> Renderer configured (VSync: On, Letterbox: %dx%d).",
          WIDTH, HEIGHT);

  G::image = new ImageMgr(s->renderer);
  G::image->loads("config/assets.json");
  G::font = new FontMgr(s->renderer);
  G::font->loads("config/assets.json");
  G::font->loadText(
      std::string("text/") +
      yyjson_get_str(yyjson_doc_ptr_get(user_savefile, "/global/language")) +
      ".json");
  G::audio = new AudioMgr(s->mixer);
  G::audio->loads("config/assets.json");
  SDL_Log("[I] <Init> Game managers created, entering initial scene...");
  Scene* init_scene = new S_Init;
  if (!init_scene) {
    SDL_Log("[C] <Init> Can't create initial scene.");
    return SDL_APP_FAILURE;
  }
  G::scene = new SceneMgr(init_scene);
  SDL_Log("[I] <Init> Initialized successfully.");
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* _state, SDL_Event* event) {
  AppState* s = static_cast<AppState*>(_state);
  if (event->type == SDL_EVENT_QUIT) {
    SDL_Log("[I] <Event> Quit requested by user.");
    return SDL_APP_SUCCESS;
  }
  SDL_ConvertEventToRenderCoordinates(s->renderer, event);
  // input
  G::scene->getCurrentScene()->input(event);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* _state) {
  AppState* s = static_cast<AppState*>(_state);
  uint64_t current_tick = SDL_GetTicksNS();
  if (s->last_tick == 0) s->last_tick = current_tick;

  // Delta Time
  float delta = (current_tick - s->last_tick) / 1e6f;
  s->last_tick = current_tick;
  // update
  G::scene->getCurrentScene()->update(delta);
  SDL_SetRenderDrawColor(s->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(s->renderer);
  // draw
  G::scene->getCurrentScene()->draw();
  SDL_RenderPresent(s->renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* _state, SDL_AppResult result) {
  AppState* s = static_cast<AppState*>(_state);
  SDL_Log("[I] <Quit> Cleaning up resources...");

  delete G::scene;
  G::scene = nullptr;
  delete G::audio;
  G::audio = nullptr;
  delete G::font;
  G::font = nullptr;
  delete G::image;
  G::image = nullptr;
  if (s->mixer) ma_engine_uninit(s->mixer);
  delete s->mixer;
  if (s->renderer) SDL_DestroyRenderer(s->renderer);
  if (s->window) SDL_DestroyWindow(s->window);
  delete s;
  SDL_Log("[I] <Quit> All resources cleaned up.");

  if (result == SDL_APP_FAILURE) {
    SDL_Log("--- Game End With Error ---");
  } else {
    SDL_Log("--- Game End Successfully ---");
  }
}