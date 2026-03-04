#include "text.hpp"

#include "font.hpp"

Text::Text(Font* font, const std::string& content)
    : font(font), content(content) {}

    