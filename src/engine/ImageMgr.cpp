#include "ImageMgr.hpp"

#include <yyjson.h>

ImageMgr::ImageMgr(SDL_Renderer* renderer) : renderer(renderer) {}
bool ImageMgr::load(const std::string& name, const std::string& path,
                    int frame_count) {
  SDL_Surface* surface = SDL_LoadPNG(path.c_str());
  if (!surface) {
    SDL_Log("[E] <ImageMgr - load> Can't load image '%s': %s", path.c_str(),
            SDL_GetError());
    return false;
  }
  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
  if (!tex) {
    SDL_Log("[E] <ImageMgr - load> Can't load image '%s': %s", path.c_str(),
            SDL_GetError());
    SDL_DestroySurface(surface);
    return false;
  }
  SDL_DestroySurface(surface);
  image_assets[name] = {tex, frame_count};
  return true;
}
bool ImageMgr::loads(const std::string& config_path) {
  size_t size;
  void* config_data = SDL_LoadFile(config_path.c_str(), &size);
  if (!config_data) {
    SDL_Log("[E] <ImageMgr - loads> Can't open json file '%s': %s",
            config_path.c_str(), SDL_GetError());
    return false;
  }
  yyjson_read_err err;
  yyjson_doc* config =
      yyjson_read_opts((char*)config_data, size, 0, nullptr, &err);
  if (!config) {
    SDL_free(config_data);
    SDL_Log("[E] <ImageMgr - loads> Can't parse json file %s: %s",
            config_path.c_str(), err.msg);
    return false;
  }
  yyjson_val* images = yyjson_doc_ptr_get(config, "/image");
  if (!images || !yyjson_is_arr(images)) {
    SDL_Log("[E] <ImageMgr - loads> No images found in %s",
            config_path.c_str());
    return false;
  }
  SDL_Log("[I] <ImageMgr - loads> Loading images in %s...",
          config_path.c_str());
  size_t idx, max;
  yyjson_val* image;
  yyjson_arr_foreach(images, idx, max, image) {
    if (!yyjson_is_obj(image)) continue;
    const char* name = yyjson_get_str(yyjson_obj_get(image, "name"));
    const char* path = yyjson_get_str(yyjson_obj_get(image, "path"));
    if (!name || !path) continue;
    int frame = yyjson_get_int(yyjson_obj_get(image, "frame"));
    if (frame <= 0) frame = 1;
    if (load(name, std::string("image/") + path, frame))
      SDL_Log("[I] <ImageMgr - loads> Loaded image '%s' (%d frames).", name,
              frame);
    else
      SDL_Log("[I] <ImageMgr - loads> Failed to load image '%s'.", name);
  }
  yyjson_doc_free(config);
  SDL_free(config_data);
  return true;
}
int ImageMgr::getFrame(const std::string& name) {
  if (!image_assets.count(name)) {
    SDL_Log("[E] <ImageMgr - getFrame> Can't find image '%s'.", name.c_str());
    return 1;
  }
  return image_assets.find(name)->second.second;
}
void ImageMgr::draw(const std::string& name, float x, float y, float alpha,
                    int frame_idx, float scale) {
  if (!image_assets.count(name)) {
    SDL_Log("[E] <ImageMgr - draw> Can't find image '%s'.", name.c_str());
    return;
  }
  auto& [_, obj] = *image_assets.find(name);
  SDL_Texture* tex = obj.first;
  int frame_count = obj.second;
  if (frame_idx < 0 || frame_idx >= frame_count) {
    SDL_Log("[E] <ImageMgr - draw> Can't draw image '%s': frame index out of range",
            name.c_str());
    return;
  }
  // 横向平铺，x坐标为 total_x * frame_idx / frame_count
  float src_w, src_h;
  SDL_GetTextureSize(tex, &src_w, &src_h);
  SDL_FRect src = {src_w * frame_idx / frame_count, 0.0f, src_w / frame_count,
                   src_h};
  SDL_FRect dst = {x, y, src_w * scale / frame_count, src_h * scale};
  SDL_SetTextureAlphaModFloat(tex, alpha);
  SDL_RenderTexture(renderer, tex, &src, &dst);
}

ImageMgr::~ImageMgr() {
  for (auto& [_, obj] : image_assets) {
    SDL_DestroyTexture(obj.first);
  }
  image_assets.clear();
}