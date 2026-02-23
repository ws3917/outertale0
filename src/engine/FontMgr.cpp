#include "FontMgr.hpp"

#include <yyjson.h>

#include <algorithm>

FontMgr::FontMgr(SDL_Renderer* renderer) : renderer(renderer) {}
bool FontMgr::load(const std::string& name, const std::string& path) {
  size_t data_size;
  void* raw_data = SDL_LoadFile(path.c_str(), &data_size);
  if (!raw_data) {
    SDL_Log("[E] <FontMgr - load> Fnt file is not found: %s", path.c_str());
    return false;
  }
  uint8_t* file = static_cast<uint8_t*>(raw_data);
  uint8_t* end = file + data_size;

  // 加载字图
  std::string glyph_path = path;
  size_t pos = glyph_path.find(".fnt");
  if (pos != std::string::npos) {
    glyph_path.replace(pos, 4, "_0.png");
  }
  SDL_Surface* glyph_file = SDL_LoadPNG(glyph_path.c_str());
  if (!glyph_file) {
    SDL_Log("[E] <FontMgr - load> Can't open glyph file %s: %s", glyph_path.c_str(),
            SDL_GetError());
    SDL_free(raw_data);
    return false;
  }
  Font* font = new Font;
  font->atlas = SDL_CreateTextureFromSurface(renderer, glyph_file);
  SDL_SetTextureScaleMode(font->atlas, SDL_SCALEMODE_PIXELART);
  SDL_DestroySurface(glyph_file);

  // 文件头
  uint8_t valid_header[4] = {'B', 'M', 'F', 3};
  for (int i = 0; i < 4; i++) {
    uint8_t value = read<uint8_t>(file);
    if (value != valid_header[i]) {
      SDL_Log("[E] <FontMgr - load> Invalid fnt Header.");
      return false;
    }
  }

  // 读物内容
  while (file < end) {
    uint8_t block_type = read<uint8_t>(file);
    uint32_t block_size = read<uint32_t>(file);
    uint8_t* next_block_pos = file + block_size;
    if (next_block_pos > end) {
      SDL_Log("[E] <FontMgr - load> Block size overflow.");
      break;
    }
    int num_chars = block_size / 20;

    switch (block_type) {
      case 2:
        font->line_height = read<uint16_t>(file);
        font->base = read<uint16_t>(file);
        file += 11;
        break;
      case 4:
        for (int i = 0; i < num_chars; i++) {
          Font::Glyph g;
          g.id = read<uint32_t>(file);
          g.x = read<uint16_t>(file);
          g.y = read<uint16_t>(file);
          g.width = read<uint16_t>(file);
          g.height = read<uint16_t>(file);
          g.x_offset = read<int16_t>(file);
          g.y_offset = read<int16_t>(file);
          g.x_advance = read<int16_t>(file);
          file += 2;
          font->glyphs[g.id] = g;
        }
        break;
      default:
        break;
    }
    file = next_block_pos;
  }
  SDL_free(raw_data);
  font_assets[name] = font;
  return true;
}
bool FontMgr::loadText(const std::string& config_path) {
  size_t size;
  void* config_data = SDL_LoadFile(config_path.c_str(), &size);
  if (!config_data) {
    SDL_Log("[E] <FontMgr - loadText> Can't open json file '%s': %s", config_path.c_str(),
            SDL_GetError());
    return false;
  }
  // 清理已打开文件
  if (text_table) {
    SDL_Log("[I] <FontMgr - loadText> Found existing text table, will clean it.");
    yyjson_doc_free(text_table);
    text_table = nullptr;
  }
  yyjson_read_err err;
  text_table = yyjson_read_opts((char*)config_data, size, 0, nullptr, &err);
  if (!text_table) {
    SDL_free(config_data);
    SDL_Log("[E] <FontMgr - loadText> Can't parse json file '%s': %s",
            config_path.c_str(), err.msg);
    return false;
  }
  return true;
}
std::string FontMgr::format(const std::string& text) {
  if (!text_table || text.find("${") == std::string::npos) {
    return text;
  }

  std::string result;
  result.reserve(text.size() * 1.5);  // 预分配一点空间

  for (size_t i = 0; i < text.size(); ++i) {
    bool replaced = false;
    if (text[i] == '$' && i + 1 < text.size() && text[i + 1] == '{') {
      size_t start = i + 2;
      size_t end = text.find('}', start);
      if (end != std::string::npos) {
        std::string key = "." + text.substr(start, end - start);
        std::replace(key.begin(), key.end(), '.', '/');
        yyjson_val* val = yyjson_doc_ptr_get(text_table, key.c_str());
        if (val && yyjson_is_str(val)) {
          result += yyjson_get_str(val);
          i = end;
          replaced = true;
        }
      }
    }
    if (!replaced) {
      result += text[i];
    }
  }
  return result;
}
void FontMgr::draw(const std::string& name, const std::string& text, float x,
                   float y, SDL_Color color, float scale) {
  if (!font_assets.count(name)) {
    SDL_Log("[E] <FontMgr - draw> Can't find font '%s'.", name.c_str());
    return;
  }

  Font* font = font_assets.find(name)->second;
  SDL_SetTextureColorMod(font->atlas, color.r, color.g, color.b);
  float cur_x = x, cur_y = y;
  for (size_t i = 0; i < text.length();) {
    uint32_t code = getNextUTF8(text, i);

    // 换行
    if (code == '\n') {
      cur_x = x;
      cur_y += font->line_height * scale;
      continue;
    }
    // 字符不存在
    if (font->glyphs.find(code) == font->glyphs.end()) {
      SDL_Log("[E] <FontMgr - draw> missing character: U+%x", code);
      continue;
    }
    const Font::Glyph& g = font->glyphs[code];
    SDL_FRect src_rect = {1.0f * g.x, 1.0f * g.y, 1.0f * g.width,
                          1.0f * g.height};
    SDL_FRect dst_rect = {cur_x + g.x_offset * scale,
                          cur_y + g.y_offset * scale, scale * g.width,
                          scale * g.height};
    SDL_RenderTexture(renderer, font->atlas, &src_rect, &dst_rect);
    cur_x += g.x_advance * scale;
  }
}
uint32_t FontMgr::getNextUTF8(const std::string& str, size_t& i) {
  uint8_t c = (uint8_t)str[i];
  // 1 byte
  if (c < 0x80) return str[i++];
  // 2 bytes
  if ((c & 0xE0) == 0xC0) {
    uint32_t res = (str[i++] & 0x1F) << 6;
    res |= str[i++] & 0x3F;
    return res;
  }
  // 3 bytes
  if ((c & 0xF0) == 0xE0) {
    uint32_t res = (str[i++] & 0x0F) << 12;
    res |= (str[i++] & 0x3F) << 6;
    res |= str[i++] & 0x3F;
    return res;
  }
  return '?';
}
bool FontMgr::loads(const std::string& config_path) {
  size_t size;
  void* config_data = SDL_LoadFile(config_path.c_str(), &size);
  if (!config_data) {
    SDL_Log("[E] <FontMgr - loads> Can't open json file '%s': %s",
            config_path.c_str(), SDL_GetError());
    return false;
  }
  yyjson_read_err err;
  yyjson_doc* config =
      yyjson_read_opts((char*)config_data, size, 0, nullptr, &err);
  if (!config) {
    SDL_free(config_data);
    SDL_Log("[E] <FontMgr - loads> Can't parse json file %s: %s",
            config_path.c_str(), err.msg);
    return false;
  }

  yyjson_val* fonts = yyjson_doc_ptr_get(config, "/font");
  if (!fonts || !yyjson_is_arr(fonts)) {
    SDL_Log("[E] <FontMgr - loads> No fonts found in %s", config_path.c_str());
    return false;
  }
  SDL_Log("[I] <FontMgr - loads> Loading fonts in %s...", config_path.c_str());
  size_t idx, max;
  yyjson_val* font;
  yyjson_arr_foreach(fonts, idx, max, font) {
    if (!yyjson_is_obj(font)) continue;
    const char* name = yyjson_get_str(yyjson_obj_get(font, "name"));
    const char* path = yyjson_get_str(yyjson_obj_get(font, "path"));
    if (!name || !path) continue;
    if (load(name, std::string("font/") + path))
      SDL_Log("[I] <FontMgr - loads> Loaded font '%s'.", name);
    else
      SDL_Log("[E] <FontMgr - loads> Failed to load font '%s'.", name);
  }
  yyjson_doc_free(config);
  SDL_free(config_data);
  return true;
}
FontMgr::~FontMgr() {
  if (text_table) {
    yyjson_doc_free(text_table);
    text_table = nullptr;
  }
  for (auto& [_, font] : font_assets) {
    if (font->atlas) {
      SDL_DestroyTexture(font->atlas);
      font->atlas = nullptr;
    }
    font->glyphs.clear();
    delete font;
  }
  font_assets.clear();
}