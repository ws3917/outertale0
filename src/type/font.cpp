#include "font.hpp"

#include <SDL3/SDL.h>

#include <filesystem>

#include "texture.hpp"

void Font::reset() {
  Object::reset();
  texture.reset();
  glyphs.clear();
  metadata = {};
}
bool Font::load(const std::string& filepath) {
  if (loaded) reset();
  if (!Object::load(filepath)) return false;
  loaded = false;
  std::unordered_map<int, Glyph> new_glyphs = {};
  const uint8_t *file = data.data(), *end = data.data() + data.size();
  if (data.size() < 4 || file[0] != 'B' || file[1] != 'M' || file[2] != 'F' ||
      file[3] != 3) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid FNT format");
    return {};
  }
  file += 4;
  while (file < end) {
    if (end - file < 5) break;
    uint8_t block_type = read<uint8_t>(file);
    uint32_t block_size = read<uint32_t>(file);
    const uint8_t* next_block = file + block_size;
    if (next_block > end) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Block overflow");
      break;
    }
    switch (block_type) {
      case 2:
        metadata.line_height = read<uint16_t>(file);
        metadata.base = read<uint16_t>(file);
        break;
      case 3:
        metadata.page_filename = reinterpret_cast<const char*>(file);
        break;
      case 4: {
        int num_chars = block_size / 20;
        new_glyphs.reserve(num_chars);
        for (int i = 0; i < num_chars; i++) {
          uint32_t id = read<uint32_t>(file);
          new_glyphs.emplace(id, Glyph{.x = read<uint16_t>(file),
                                       .y = read<uint16_t>(file),
                                       .w = read<uint16_t>(file),
                                       .h = read<uint16_t>(file),
                                       .x_offset = read<int16_t>(file),
                                       .y_offset = read<int16_t>(file),
                                       .x_advance = read<int16_t>(file)});
          file += 2;
        }
        break;
      }
      default:
        break;
    }
    file = next_block;
  }
  if (new_glyphs.empty()) return false;
  std::filesystem::path font_path(filepath);
  std::string texture_path =
      (font_path.parent_path() / metadata.page_filename).string();
  Texture new_texture(renderer);
  if (!new_texture.load(texture_path)) return false;
  // 转移资源所有权
  glyphs = std::move(new_glyphs);
  texture = std::move(new_texture);
  loaded = true;
  return true;
}