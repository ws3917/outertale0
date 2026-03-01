// save metadata
#include <SDL3/SDL.h>

#include <string>
#include <unordered_map>

struct Save {
  struct {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
  } meta;
  std::unordered_map<std::string, bool> flag_bool = {
      {"fullscreen", false},
  };
  std::unordered_map<std::string, float> flag_num = {
      {"music_volume", 0.7f},
      {"sound_volume", 0.7f},
  };
  std::unordered_map<std::string, std::string> flag_str = {
      {"language", "zh_CN"},
  };
};