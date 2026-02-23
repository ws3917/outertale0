// 资源管理器
#pragma once

#include <SDL3/SDL.h>

#include <string>
#include <unordered_map>

class yyjson_doc;
struct Font {
  struct Glyph {
    uint32_t id;
    uint16_t x, y, width, height;
    int16_t x_offset, y_offset;
    int16_t x_advance;
  };
  int line_height = 0;  // 基准行高
  int base = 0;         // 基准线
  std::unordered_map<uint32_t, Glyph> glyphs = {};
  SDL_Texture* atlas = nullptr;
};

class FontMgr final {
 public:
  bool load(const std::string& name, const std::string& path);
  bool loadText(const std::string& config_path);
  bool loads(const std::string& config_path);
  std::string format(const std::string& text);
  void draw(const std::string& name, const std::string& text, float x, float y,
            SDL_Color color, float scale);
  FontMgr(SDL_Renderer* renderer);
  ~FontMgr();

 private:
  // 读取str[i]位置的下一个UTF-8字符
  uint32_t getNextUTF8(const std::string& str, size_t& i);
  yyjson_doc* text_table = nullptr;

  template <typename T>
  T read(uint8_t*& cursor) {
    T value;
    memcpy(&value, cursor, sizeof(T));
    cursor += sizeof(T);
    return value;
  }
  std::unordered_map<std::string, Font*> font_assets;
  SDL_Renderer* renderer;
};