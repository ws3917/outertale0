#include "utils.hpp"

#include <physfs.h>

#include <nlohmann/json.hpp>

SDL_IOStream* U::IOFromFile(const std::string& filepath) {
  const std::string full_path = "assets/" + filepath;
  PHYSFS_File* file = PHYSFS_openRead(full_path.c_str());
  if (!file) return nullptr;

  PHYSFS_sint64 size = PHYSFS_fileLength(file);
  void* buffer = SDL_malloc(size);
  if (!buffer) {
    PHYSFS_close(file);
    return nullptr;
  }

  if (PHYSFS_readBytes(file, buffer, size) < size) {
    SDL_free(buffer);
    PHYSFS_close(file);
    return nullptr;
  }
  PHYSFS_close(file);
  return SDL_IOFromMem(buffer, size);
}
nlohmann::json U::LoadJson(const std::string& filepath) {
  SDL_IOStream* io = IOFromFile(filepath);
  if (!io) {
    return nlohmann::json::object();  // 返回空对象而非崩溃
  }
  Sint64 size = SDL_GetIOSize(io);
  std::vector<uint8_t> buffer(static_cast<size_t>(size));
  SDL_ReadIO(io, buffer.data(), buffer.size());
  SDL_CloseIO(io);
  try {
    nlohmann::json j = nlohmann::json::parse(buffer);
    return j;
  } catch (const nlohmann::json::parse_error& e) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
    return nlohmann::json::object();
  }
}
std::unordered_map<int, U::Glyph> U::LoadGlyph(const std::string& filepath,
                                               GlyphMetadata& metadata) {
  SDL_IOStream* io = IOFromFile(filepath);
  if (!io) {
    // 无数据
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't load font glyph.");
    return {};
  }
  Sint64 size = SDL_GetIOSize(io);
  std::vector<uint8_t> buffer(static_cast<size_t>(size));
  SDL_ReadIO(io, buffer.data(), buffer.size());
  SDL_CloseIO(io);
  std::unordered_map<int, Glyph> glyphs;
  const uint8_t *file = buffer.data(), *end = buffer.data() + buffer.size();
  if (buffer.size() < 4 || file[0] != 'B' || file[1] != 'M' || file[2] != 'F' ||
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
        glyphs.reserve(num_chars);
        for (int i = 0; i < num_chars; i++) {
          uint32_t id = read<uint32_t>(file);
          glyphs.emplace(id, Glyph{.x = read<uint16_t>(file),
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
  return glyphs;
}
SDL_Texture* U::LoadTexture(const std::string& filepath,
                            SDL_Renderer* renderer) {
  SDL_IOStream* io = IOFromFile(filepath);
  if (!io) {
    // 无数据
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't load image file.");
    return nullptr;
  }
  SDL_Surface* surface = SDL_LoadPNG_IO(io, true);
  if (!surface) {
    SDL_CloseIO(io);
    return nullptr;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);
  return texture;
}

MIX_Audio* U::LoadAudio(const std::string& filepath, MIX_Mixer* mixer) {
  SDL_IOStream* io = IOFromFile(filepath);
  if (!io) {
    // 无数据
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Can't load audio file.");
    return nullptr;
  }
  MIX_Audio* audio = MIX_LoadAudio_IO(mixer, io, true, true);
  if (!audio) {
    SDL_CloseIO(io);
    return nullptr;
  }
  return audio;
}
// 获取颜色
SDL_Color U::GetColor(char c) {
  // 红橙黄绿青蓝紫黑白
  switch (c) {
    case 'R':
      return {255, 0, 0, 255};
    case 'O':
      return {255, 165, 0, 255};
    case 'Y':
      return {255, 255, 0, 255};
    case 'G':
      return {0, 255, 0, 255};
    case 'C':
      return {0, 200, 255, 255};
    case 'B':
      return {30, 80, 255, 255};
    case 'P':
      return {120, 40, 255, 255};
    case 'K':
      return {0, 0, 0, 255};
    case 'W':
      return {255, 255, 255, 255};
    default:
      return {255, 255, 255, 255};
  }
}