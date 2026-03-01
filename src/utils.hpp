#include <SDL3/SDL.h>
#include <physfs.h>

#include <string>

namespace U {
inline SDL_IOStream* IOFromFile(const std::string& filepath) {
  const std::string full_path = "assets/" + filepath;
  PHYSFS_File* file = PHYSFS_openRead(full_path.c_str());
  if (file == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to open file. PhysFS Error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return nullptr;
  }

  PHYSFS_sint64 size = PHYSFS_fileLength(file);
  if (size < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to get file size. PhysFS Error: %s",
                 PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return nullptr;
  }
  char* buffer = (char*)SDL_malloc(size);
  PHYSFS_readBytes(file, buffer, size);
  PHYSFS_close(file);
  return SDL_IOFromConstMem(buffer, size);
}
}  // namespace U