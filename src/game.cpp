#include "game.hpp"

#include <SDL3_mixer/SDL_mixer.h>

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
  // load();
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

void Game::AssetDeleter::operator()(SDL_Window* window) const {
  if (window) SDL_DestroyWindow(window);
}
void Game::AssetDeleter::operator()(SDL_Renderer* renderer) const {
  if (renderer) SDL_DestroyRenderer(renderer);
}
void Game::AssetDeleter::operator()(MIX_Mixer* mixer) const {
  if (mixer) MIX_DestroyMixer(mixer);
}