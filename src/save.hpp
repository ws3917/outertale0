#pragma once
// save metadata
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <string>
#include <unordered_map>

struct Save {
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