#include "texture.hpp"

#include <SDL3/SDL.h>

void Texture::SDL_TextureDeleter::operator()(SDL_Texture* texture) const {
  if (texture) SDL_DestroyTexture(texture);
}
SDL_FPoint Texture::getSize() { return {w, h}; }
void Texture::reset() {
  Object::reset();
  texture.reset();
  loaded = false;
  w = h = 0.0f;
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
  SDL_GetTextureSize(texture.get(), &w, &h);
  loaded = true;
  return true;
}
void Texture::draw(float x, float y, float scale) {
  if (!loaded || !texture) return;
  SDL_FRect dst_rect = {x, y, w * scale, h * scale};
  SDL_RenderTexture(renderer, texture.get(), nullptr, &dst_rect);
}
void Texture::drawEx(const SDL_FRect* src, const SDL_FRect* dst, uint8_t alpha,
                     double angle, SDL_FPoint* center, SDL_FlipMode flip) {
  if (!loaded || !texture) return;
  if (alpha != 255) {
    SDL_SetTextureAlphaMod(texture.get(), alpha);
  }
  SDL_RenderTextureRotated(renderer, texture.get(), src, dst, angle, center,
                           flip);
  if (alpha != 255) {
    SDL_SetTextureAlphaMod(texture.get(), 255);
  }
}