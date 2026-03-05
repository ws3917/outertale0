#pragma once
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <memory>

#include "res.hpp"
#include "scene/scene.hpp"

class Game {
 public:
  Game();
  ~Game();
  void run();

  void changeScene(std::unique_ptr<Scene> new_scene);

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
  std::unique_ptr<Scene> current_scene;
  Res res;
};