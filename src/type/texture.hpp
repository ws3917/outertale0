#pragma once
#include "object.hpp"
class SDL_Renderer;
class SDL_Texture;
class Texture : public Object {
 public:
  Texture(SDL_Renderer* renderer) : renderer(renderer) {}
  bool load(const std::string& filepath) override;
  void reset() override;

 private:
  struct SDL_TextureDeleter {
    void operator()(SDL_Texture* texture) const;
  };
  SDL_Renderer* renderer = nullptr;
  std::unique_ptr<SDL_Texture, SDL_TextureDeleter> texture;
};