#pragma once
#include "utils.hpp"
class Font {
 public:
  Font(SDL_Renderer* renderer, const std::string& filepath = nullptr);
  SDL_Renderer* renderer = nullptr;
  bool load(const std::string& filepath);

 private:
  U::GlyphMetadata metadata = {};
  std::unordered_map<int, U::Glyph> glyphs = {};
  SDL_Texture* texture = nullptr;
  bool loaded = false;
};