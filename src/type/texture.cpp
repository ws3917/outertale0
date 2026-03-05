#include "texture.hpp"

#include <SDL3/SDL.h>

void Texture::SDL_TextureDeleter::operator()(SDL_Texture* texture) const {
  if (texture) SDL_DestroyTexture(texture);
}
void Texture::reset() {
  Object::reset();
  texture.reset();
  loaded = false;
}
bool Texture::load(const std::string& filepath) {
  if (loaded) reset();
  if (!Object::load(filepath)) return false;
  loaded = false;
  SDL_SeekIO(io.get(), 0, SDL_IO_SEEK_SET);
  SDL_Surface* surface = SDL_LoadPNG_IO(io.get(), false);
  if (!surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create surface: %s",
                 SDL_GetError());
    return false;
  }
  texture.reset(SDL_CreateTextureFromSurface(renderer, surface));
  SDL_DestroySurface(surface);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s",
                 SDL_GetError());
    return false;
  }
  loaded = true;
  return true;
}