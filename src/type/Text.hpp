#pragma once
#include "../engine/FontMgr.hpp"
#include "../engine/Global.hpp"
#include "../engine/Object.hpp"

class Text : public Object {
 public:
  Text(std::string src, std::string text = "") : src(src), text(text) {}
  void setText(std::string text) { this->text = text; }
  void setColor(SDL_Color color) { this->color = color; }
  void draw() override { G::font->draw(src, text, x, y, color, scale); }

 private:
  SDL_Color color = {255, 255, 255, 255};
  float scale = 1.0;
  std::string src;
  std::string text;
};