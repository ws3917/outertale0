#pragma once
#include <memory>

#include "type/object.hpp"

class SDL_Window;
class SDL_Renderer;
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

class GameContext {
 public:
  GameContext();
  ~GameContext();
};