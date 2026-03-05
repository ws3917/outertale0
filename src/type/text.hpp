#pragma once
#include <string>
class Font;
class SDL_Color;
class Text {
 public:
  Text(Font* font, const std::string& content = "");
  void draw(float x, float y);

 private:
  Font* font;
  std::string content;
  SDL_Color getColor(char ch);
};