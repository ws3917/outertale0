#pragma once
#include <SDL3/SDL.h>

#include "object.hpp"

class Texture : public Object {
 public:
  Texture(SDL_Renderer* renderer) : renderer(renderer) {}
  bool load(const std::string& filepath) override;
  void reset() override;
  void draw(float x, float y, float scale = 1.0);
  SDL_FPoint getSize();
  void drawEx(const SDL_FRect* src, const SDL_FRect* dst, uint8_t alpha = 255,
              double angle = 0.0, SDL_FPoint* center = nullptr,
              SDL_FlipMode flip = SDL_FLIP_NONE);

 private:
  struct SDL_TextureDeleter {
    void operator()(SDL_Texture* texture) const;
  };
  SDL_Renderer* renderer = nullptr;
  std::unique_ptr<SDL_Texture, SDL_TextureDeleter> texture;
  float w = 0.0f, h = 0.0f;
};