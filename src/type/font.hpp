#pragma once
#include <unordered_map>

#include "texture.hpp"

class Font : public Object {
 public:
  Font(SDL_Renderer* renderer) : texture(renderer), renderer(renderer) {}
  bool load(const std::string& filepath) override;
  void reset() override;

 private:
  struct Glyph {
    uint16_t x, y, w, h;
    int16_t x_offset, y_offset, x_advance;
  };
  struct GlyphMetadata {
    uint16_t line_height;
    uint16_t base;
    std::string page_filename;
  };
  template <typename T>
  T read(const uint8_t*& ptr) {
    T value;
    std::memcpy(&value, ptr, sizeof(T));
    ptr += sizeof(T);
    return value;
  }
  Texture texture;
  SDL_Renderer* renderer;
  GlyphMetadata metadata = {};
  std::unordered_map<int, Glyph> glyphs = {};
};