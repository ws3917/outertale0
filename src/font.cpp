#include "font.hpp"

#include <filesystem>

Font::Font(SDL_Renderer* renderer, const std::string& filepath)
    : renderer(renderer) {
  if (!filepath.empty()) load(filepath);
}
bool Font::load(const std::string& filepath) {
  if (loaded) {
    SDL_DestroyTexture(texture);
    glyphs.clear();
    loaded = false;
  }
  if (filepath.empty()) return false;
  loaded = false;

  auto new_glyphs = U::LoadGlyph(filepath, metadata);
  if (new_glyphs.empty()) return false;
  std::filesystem::path font_path(filepath);
  std::string texture_path =
      (font_path.parent_path() / metadata.page_filename).string();
  SDL_Texture* new_texture = U::LoadTexture(texture_path, renderer);
  if (!new_texture) return false;
  // 转移资源所有权
  glyphs = std::move(new_glyphs);
  texture = new_texture;
  loaded = true;
  return true;
}