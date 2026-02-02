#include "E_Font.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
bool E_Font::load(SDL_Renderer *renderer, const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        SDL_Log("[C] <LoadFont> Fnt file not found: %s", path.c_str());
        return false;
    }

    // 加载字图
    std::string glyph_path = path;
    size_t pos = glyph_path.find(".fnt");
    if (pos != std::string::npos)
    {
        glyph_path.replace(pos, 4, "_0.png");
    }
    SDL_Surface *glyph_file = SDL_LoadPNG(glyph_path.c_str());
    if (!glyph_file)
    {
        SDL_Log("[C] <LoadFont> Can't open glyph file %s: %s", glyph_path.c_str(), SDL_GetError());
        return false;
    }
    atlas = SDL_CreateTextureFromSurface(renderer, glyph_file);
    SDL_SetTextureScaleMode(atlas, SDL_SCALEMODE_PIXELART);
    SDL_DestroySurface(glyph_file);

    // 文件头
    uint8_t valid_header[4] = {'B', 'M', 'F', 3};
    for (int i = 0; i < 4; i++)
    {
        uint8_t value = read<uint8_t>(file);
        if (value != valid_header[i])
        {
            SDL_Log("[C] <LoadFont> Invalid fnt Header.");
            return false;
        }
    }

    // 读物内容
    while (file.peek() != EOF)
    {
        uint8_t block_type = read<uint8_t>(file);
        uint32_t block_size = read<uint32_t>(file);
        int num_chars = block_size / 20;
        std::streampos next_block_pos = file.tellg() + std::streamoff(block_size);

        switch (block_type)
        {
        case 2:
            line_height = read<uint16_t>(file);
            base = read<uint16_t>(file);
            read<uint16_t>(file);
            read<uint16_t>(file);
            read<uint16_t>(file);
            read<uint8_t>(file);
            read<uint8_t>(file);
            read<uint8_t>(file);
            read<uint8_t>(file);
            read<uint8_t>(file);
            break;
        case 4:
            for (int i = 0; i < num_chars; i++)
            {
                Glyph g;
                g.id = read<uint32_t>(file);
                g.x = read<uint16_t>(file);
                g.y = read<uint16_t>(file);
                g.width = read<uint16_t>(file);
                g.height = read<uint16_t>(file);
                g.x_offset = read<int16_t>(file);
                g.y_offset = read<int16_t>(file);
                g.x_advance = read<int16_t>(file);
                read<uint8_t>(file);
                read<uint8_t>(file);
                glyphs[g.id] = g;
            }
            break;
        default:
            break;
        }
        file.seekg(next_block_pos);
    }

    return true;
}

uint32_t E_Font::getNextUTF8(const std::string &str, size_t &i)
{
    uint8_t c = (uint8_t)str[i];
    // 1 byte
    if (c < 0x80)
        return str[i++];
    // 2 bytes
    if ((c & 0xE0) == 0xC0)
    {
        uint32_t res = (str[i++] & 0x1F) << 6;
        res |= str[i++] & 0x3F;
        return res;
    }
    // 3 bytes
    if ((c & 0xF0) == 0xE0)
    {
        uint32_t res = (str[i++] & 0x0F) << 12;
        res |= (str[i++] & 0x3F) << 6;
        res |= str[i++] & 0x3F;
        return res;
    }
    return '?';
}

void E_Font::draw(SDL_Renderer *renderer, const std::string &text, float x, float y, float scale)
{
    float cur_x = x, cur_y = y;
    for (size_t i = 0; i < text.length();)
    {
        uint32_t code = getNextUTF8(text, i);

        // 换行
        if (code == '\n')
        {
            cur_x = x;
            cur_y += line_height * scale;
            continue;
        }
        // 字符不存在
        if (glyphs.find(code) == glyphs.end())
        {
            SDL_Log("[E] <DrawFont> missing character: U+%x", code);
            continue;
        }
        const Glyph &g = glyphs[code];
        SDL_FRect src_rect = {1.0f * g.x, 1.0f * g.y, 1.0f * g.width, 1.0f * g.height};
        SDL_FRect dst_rect = {cur_x + g.x_offset * scale, cur_y + g.y_offset * scale, scale * g.width,
                              scale * g.height};
        SDL_RenderTexture(renderer, atlas, &src_rect, &dst_rect);
        cur_x += g.x_advance * scale;
    }
}