#pragma once
#include "../engine/FontMgr.hpp"
#include "../engine/Global.hpp"
#include "Object.hpp"

class Text : public Object {
 public:
  Text(std::string src, std::string text = "") : src(src) { setText(text); }
  void setText(std::string text) {
    if (this->raw_text == text) return;
    this->raw_text = text;
    this->text = G::font->format(this->raw_text);
  }

  void setColor(SDL_Color color) { this->color = color; }
  void draw() override { G::font->draw(src, text, x, y, color, scale); }
  float scale = 1.0;

 private:
  SDL_Color color = {255, 255, 255, 255};
  std::string src;
  std::string raw_text;
  std::string text;
};