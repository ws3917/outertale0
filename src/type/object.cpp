#include "object.hpp"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <physfs.h>

void Object::SDL_IOCloseDeleter::operator()(SDL_IOStream* io) const {
  if (io) SDL_CloseIO(io);
}
void Object::reset() {
  SDL_CloseIO(io.get());
  data.clear();
  loaded = false;
}
bool Object::load(const std::string& filepath) {
  if (loaded) reset();
  const std::string full_path = "assets/" + filepath;
  PHYSFS_File* file = PHYSFS_openRead(full_path.c_str());
  if (!file) return false;

  PHYSFS_sint64 size = PHYSFS_fileLength(file);
  std::vector<uint8_t> buffer;
  try {
    buffer.resize(size);
  } catch (const std::bad_alloc& e) {
    SDL_Log("Memory allocation failed: %s", e.what());
    PHYSFS_close(file);
    return false;
  }

  if (PHYSFS_readBytes(file, buffer.data(), size) < size) {
    PHYSFS_close(file);
    return false;
  }
  PHYSFS_close(file);
  data = std::move(buffer);
  io.reset(SDL_IOFromMem(buffer.data(), size));
  if (!io) {
    data.clear();
    return false;
  }
  loaded = true;
  return true;
}