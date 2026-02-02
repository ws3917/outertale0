#pragma once
#include <SDL3/SDL.h>
#include <fstream>
#include <map>
#include <string>

class E_Font
{
  public:
    bool load(SDL_Renderer *renderer, const std::string &path);
    void draw(SDL_Renderer *renderer, const std::string &text, float x, float y, float scale = 1.0);

  private:
    struct Glyph
    {
        uint32_t id;
        uint16_t x, y, width, height;
        int16_t x_offset, y_offset;
        int16_t x_advance;
    };
    // 读取str[i]位置的下一个UTF-8字符
    uint32_t getNextUTF8(const std::string &str, size_t &i);

    template <typename T> T read(std::ifstream &file)
    {
        T value;
        file.read(reinterpret_cast<char *>(&value), sizeof(T));
        return value;
    }
    uint16_t line_height = 0; // 基准行高
    uint16_t base = 0;        // 基准线

    std::map<uint32_t, Glyph> glyphs = {};
    SDL_Texture *atlas = nullptr;
};