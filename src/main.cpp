#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <gjAPI.h>
#include <physfs.h>

#include <nlohmann/json.hpp>

#include "save.hpp"
#include "timer.hpp"
#include "utils.hpp"
#include "values.hpp"

int main(int, char** argv) {
  // 读取一张图片并显示
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return 1;
  if (!MIX_Init()) return 1;
  // physfs test
  bool data_mount_success = false, save_mount_success = false;
#ifdef SDL_PLATFORM_ANDROID
  if (!PHYSFS_init(reinterpret_cast<char*>(new PHYSFS_AndroidInit{
          SDL_GetAndroidJNIEnv(), SDL_GetAndroidActivity()})))
    return 1;
  data_mount_success = PHYSFS_mount(PHYSFS_getBaseDir(), "/", 0);
  save_mount_success =
      PHYSFS_mount(SDL_GetAndroidExternalStoragePath(), "/user", 1);
  PHYSFS_setWriteDir(SDL_GetAndroidExternalStoragePath());
#else
  if (!PHYSFS_init(argv[0])) return 1;
  char data_path[1024];
  SDL_snprintf(data_path, sizeof(data_path), "%smain.pak", PHYSFS_getBaseDir());
  data_mount_success = PHYSFS_mount(data_path, "/", 0);
  save_mount_success = PHYSFS_mount(
      SDL_GetPrefPath(V::GAME_AUTHOR, V::GAME_SHORTNAME), "/user", 1);
  PHYSFS_setWriteDir(SDL_GetPrefPath(V::GAME_AUTHOR, V::GAME_SHORTNAME));
#endif
  if (!data_mount_success || !save_mount_success) {
    return 1;
  }

  // json test
  auto j = U::LoadJson("text/zh_CN/system.json");
  if (!j.empty())
    SDL_Log("hello, %s", j["languages"].get<std::string>().c_str());
  // game jolt api test - 用三角猜想模拟器测试
  gjAPI api(984011, "xxxxxxxxxxxxxxxxxxxxxxx");
  SDL_Log("try to log in to gamejolt");
  int result = api.LoginNow(true, "ws3917", "yayaya");
  if (result == GJ_OK)
    SDL_Log("yay login successful");
  else
    SDL_Log("login boom");
  // window / render test
  Save save;
  SDL_CreateWindowAndRenderer(
      "hello", static_cast<int>(V::WINDOW_SCALE * V::RENDER_WIDTH),
      static_cast<int>(V::WINDOW_SCALE * V::RENDER_HEIGHT),
      SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &save.meta.window,
      &save.meta.renderer);
  SDL_SetRenderVSync(save.meta.renderer, 1);
  SDL_SetDefaultTextureScaleMode(save.meta.renderer, SDL_SCALEMODE_PIXELART);
  SDL_SetRenderLogicalPresentation(save.meta.renderer, V::RENDER_WIDTH,
                                   V::RENDER_HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  // audio test
  save.meta.mixer =
      MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  MIX_Audio* audio = U::LoadAudio("music/thousands_star.mp3", save.meta.mixer);
  MIX_PlayAudio(save.meta.mixer, audio);

  // image text
  SDL_Texture* tex = U::LoadTexture("image/bg_static.png", save.meta.renderer);
  // font glyph test
  U::GlyphMetadata metadata;
  auto glyphs = U::LoadGlyph("font/main.fnt", metadata);
  SDL_Texture* font_tex =
      U::LoadTexture("font/" + metadata.page_filename, save.meta.renderer);
  U::Glyph w = glyphs[U'你'];
  SDL_FRect w_src(w.x, w.y, w.w, w.h);
  SDL_FRect w_dst(100.0f + w.x_offset, 100.0f + w.y_offset, w.w, w.h);

  // event test
  SDL_Event event;
  bool running = true;
  // color test
  // timer test
  std::string color_list = "ROYGCBPKW";
  auto current_color = color_list.begin();
  Timer timer(1000.0f, true, [&] {
    current_color++;
    if (current_color == color_list.end()) current_color = color_list.begin();
  });
  timer.start();
  uint64_t current_tick;
  const float FREQ = static_cast<float>(SDL_GetPerformanceFrequency()) / 1000;

  save.meta.last_tick = SDL_GetPerformanceCounter();
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) running = false;
      SDL_ConvertEventToRenderCoordinates(save.meta.renderer, &event);
    }
    current_tick = SDL_GetPerformanceCounter();
    float dt = (current_tick - save.meta.last_tick) / FREQ;
    save.meta.last_tick = current_tick;
    timer.update(dt);
    SDL_Color color = U::GetColor(*current_color);
    SDL_SetRenderDrawColor(save.meta.renderer, color.r, color.g, color.b,
                           color.a);
    SDL_RenderClear(save.meta.renderer);
    // SDL_RenderTexture(save.meta.renderer, tex, nullptr, nullptr);
    SDL_RenderTexture(save.meta.renderer, font_tex, &w_src, &w_dst);
    SDL_RenderPresent(save.meta.renderer);
  }
  SDL_DestroyTexture(tex);
  MIX_DestroyAudio(audio);
  MIX_DestroyMixer(save.meta.mixer);
  SDL_DestroyRenderer(save.meta.renderer);
  SDL_DestroyWindow(save.meta.window);
  PHYSFS_deinit();
  MIX_Quit();
  SDL_Quit();
  return 0;
}