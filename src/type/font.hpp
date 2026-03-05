#pragma once
#include <unordered_map>

#include "texture.hpp"

class Font : public Object {
 public:
  struct Glyph {
    uint16_t x, y, w, h;
    int16_t x_offset, y_offset, x_advance;
  };
  struct GlyphMetadata {
    uint16_t line_height;
    uint16_t base;
    std::string page_filename;
  };
  Font(SDL_Renderer* renderer) : texture(renderer), renderer(renderer) {}
  bool load(const std::string& filepath) override;
  const Texture& getTexture() const;
  const Glyph* getGlyph(int codepoint) const;
  int getLineHeight() const;
  SDL_Renderer* getRenderer() const;
  void reset() override;

 private:
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