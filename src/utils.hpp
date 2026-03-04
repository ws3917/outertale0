#pragma once
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <cstring>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

namespace U {
struct Glyph {
  uint16_t x, y, w, h;
  int16_t x_offset, y_offset, x_advance;
};
struct GlyphMetadata {
  uint16_t line_height;
  uint16_t base;
  std::string page_filename;
};
SDL_IOStream* IOFromFile(const std::string& filepath);
nlohmann::json LoadJson(const std::string& filepath);
SDL_Texture* LoadTexture(const std::string& filepath, SDL_Renderer* renderer);
MIX_Audio* LoadAudio(const std::string& filepath, MIX_Mixer* mixer);
std::unordered_map<int, Glyph> LoadGlyph(const std::string& filepath,
                                         GlyphMetadata& metadata);
SDL_Color GetColor(char c);
template <typename T>
T read(const uint8_t*& ptr) {
  T value;
  std::memcpy(&value, ptr, sizeof(T));
  ptr += sizeof(T);
  return value;
}
}  // namespace U