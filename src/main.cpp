#include "engine/Global.hpp"
#include "scene/S_Init.hpp"

#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <miniaudio.h>

#include <filesystem>

#include "engine/AudioMgr.hpp"
#include "engine/FontMgr.hpp"
#include "engine/ImageMgr.hpp"
#include "engine/SceneMgr.hpp"

struct AppState {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  ma_engine* mixer = nullptr;
  uint64_t last_tick = 0;
  Scene* current_scene = nullptr;
};
SDL_AppResult SDL_AppInit(void** _state, int, char**) {
  // 初始化
  constexpr int WIDTH = 320;
  constexpr int HEIGHT = 240;
  constexpr const char* NAME = "Your Battle";
  SDL_Log("--- Game Start ---\n");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_Log("[C] <Init> SDL init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("[I] <Init> SDL core initialized.");
  const char* base_path = SDL_GetBasePath();
  if (base_path)
    std::filesystem::current_path(std::filesystem::path(base_path));
  AppState* s = new AppState;

  s->mixer = new ma_engine;
  if (ma_engine_init(nullptr, s->mixer) != MA_SUCCESS) {
    SDL_Log("[C] <Init> Mixer device creation failed.");
    return SDL_APP_FAILURE;
  }
  SDL_Log("[I] <Init> Mixer device created.");

  s->window = SDL_CreateWindow(
      NAME, 1280, 960, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);

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
  G::image->loads("assets.json");
  G::font = new FontMgr(s->renderer);
  G::font->loads("assets.json");
  G::font->loadText("text/zh_CN.json");
  G::audio = new AudioMgr(s->mixer);
  G::audio->loads("assets.json");
  SDL_Log("[I] <Init> Game managers created, entering initial scene...");
  Scene* init_scene = new S_Init;
  if (!init_scene) {
    SDL_Log("[C] <Init> Can't create initial scene.");
    return SDL_APP_FAILURE;
  }
  G::scene = new SceneMgr(init_scene);
  *_state = s;
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
  delete G::audio;
  delete G::font;
  delete G::image;
  if (s->mixer) ma_engine_uninit(s->mixer);
  delete s->mixer;
  if (s->renderer) SDL_DestroyRenderer(s->renderer);
  if (s->window) SDL_DestroyWindow(s->window);
  SDL_Log("[I] <Quit> All resources cleaned up.");

  if (result == SDL_APP_FAILURE) {
    SDL_Log("--- Game End With Error ---");
  } else {
    SDL_Log("--- Game End Successfully ---");
  }
}