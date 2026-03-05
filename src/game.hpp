#pragma once
#include <memory>
#include <vector>

#include "type/object.hpp"
#include <SDL3/SDL.h>

class MIX_Mixer;
class Game {
 public:
  Game();
  void run();

 private:
  struct AssetDeleter {
    void operator()(SDL_Window* window) const;
    void operator()(SDL_Renderer* renderer) const;
    void operator()(MIX_Mixer* mixer) const;
  };
  uint64_t last_tick;
  std::unique_ptr<SDL_Window, AssetDeleter> window;
  std::unique_ptr<SDL_Renderer, AssetDeleter> renderer;
  std::unique_ptr<MIX_Mixer, AssetDeleter> mixer;
  std::vector<Object> objects;
};
