// 资源管理器
#pragma once
#include <SDL3/SDL.h>

#include <string>
#include <unordered_map>

class ImageMgr final {
 public:
  bool load(const std::string& name, const std::string& path,
            int frame_count = 1);
  bool loads(const std::string& config_path);

  void draw(const std::string& name, float x, float y, float alpha,
            int frame_idx, float scale);
  int getFrame(const std::string& name);
  ImageMgr(SDL_Renderer* renderer);
  ~ImageMgr();

 private:
  std::unordered_map<std::string, std::pair<SDL_Texture*, int>> image_assets;
  SDL_Renderer* renderer;
};
