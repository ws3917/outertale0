#include <SDL3/SDL.h>
#include <physfs.h>
#include <yyjson.h>

#include <string>

namespace Utils {
inline bool isMobile() {
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_EMSCRIPTEN) || \
    defined(SDL_PLATFORM_IOS)
  return true;
#else
  return false;
#endif
}
inline void* LoadFile(const std::string& path, size_t* datasize) {
  if (isMobile()) return SDL_LoadFile(path.c_str(), datasize);
  if (PHYSFS_isInit()) {
    PHYSFS_File* file = PHYSFS_openRead(path.c_str());
    if (file) {
      PHYSFS_sint64 size = PHYSFS_fileLength(file);
      void* buffer = SDL_malloc(size + 1);

      if (buffer) {
        if (PHYSFS_readBytes(file, buffer, size) == size) {
          static_cast<char*>(buffer)[size] = '\0';
          if (datasize) *datasize = (size_t)size;
          PHYSFS_close(file);
          return buffer;
        }
        // 读取失败
        SDL_free(buffer);
      }
      PHYSFS_close(file);
    }
  }

  // fallback
  SDL_Log(
      "[I] <Utils - LoadFile> Can't Load %s from pack. Trying to load from "
      "disk.",
      path.c_str());
  return SDL_LoadFile(path.c_str(), datasize);
}
inline bool SaveFile(const std::string& src, const std::string& dst) {
  size_t size = 0;
  void* data = LoadFile(src, &size);
  if (!data) {
    SDL_Log("[E] <Utils - SaveFile> Can't save file: %s can't read",
            src.c_str());
    return false;
  }
  return SDL_SaveFile(dst.c_str(), data, size);
}

inline SDL_Surface* LoadPNG(const std::string& path) {
  size_t size = 0;
  void* data = LoadFile(path.c_str(), &size);
  if (!data) {
    SDL_Log("[E] <Utils - LoadPNG> Can't open PNG file '%s': %s", path.c_str(),
            SDL_GetError());
    return nullptr;
  }
  SDL_Surface* surface = nullptr;
  SDL_IOStream* io = SDL_IOFromConstMem(data, size);
  if (io) surface = SDL_LoadPNG_IO(io, true);
  SDL_free(data);
  return surface;
}
inline yyjson_doc* LoadJson(const std::string& path) {
  size_t size;
  void* data = LoadFile(path.c_str(), &size);
  if (!data) {
    SDL_Log("[E] <Utils - LoadJson> Can't open json file '%s': %s",
            path.c_str(), SDL_GetError());
    return nullptr;
  }
  yyjson_read_err err;
  yyjson_doc* config =
      yyjson_read_opts(static_cast<char*>(data), size, 0, nullptr, &err);
  if (!config) {
    SDL_free(data);
    SDL_Log("[E] <Utils - LoadJson> Can't parse json file %s: %s", path.c_str(),
            err.msg);
    return nullptr;
  }
  SDL_free(data);
  return config;
}
};  // namespace Utils