#include "text.hpp"

#include <SDL3/SDL.h>

#include "font.hpp"


Text::Text(Font* font, const std::string& content)
    : font(font), content(content) {}

SDL_Color Text::getColor(char c) {
  // 红橙黄绿青蓝紫黑白
  switch (c) {
    case 'R':
      return {255, 0, 0, 255};
    case 'O':
      return {255, 165, 0, 255};
    case 'Y':
      return {255, 255, 0, 255};
    case 'G':
      return {0, 255, 0, 255};
    case 'C':
      return {0, 200, 255, 255};
    case 'B':
      return {30, 80, 255, 255};
    case 'P':
      return {120, 40, 255, 255};
    case 'K':
      return {0, 0, 0, 255};
    case 'W':
      return {255, 255, 255, 255};
    default:
      return {255, 255, 255, 255};
  }
}