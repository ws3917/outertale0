// 资源管理器
#pragma once
#include "E_Font.hpp"
#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>

class E_ResManager
{
  public:
    // 单例
    static E_ResManager *get()
    {
        static E_ResManager instance;
        return &instance;
    }
    E_ResManager(const E_ResManager &) = delete;
    E_ResManager &operator=(const E_ResManager &) = delete;

    // 加载资源
    template <typename T> T *load(SDL_Renderer *renderer, const std::string &name, const std::string &path);
    template <typename T> T *get(const std::string &name);

    void clear()
    {
        for (auto &[name, tex] : texture_cache)
        {
            SDL_DestroyTexture(tex);
        }
        for (auto &[name, font] : font_cache)
        {
            delete font;
        }
        texture_cache.clear();
        font_cache.clear();
    }

  private:
    E_ResManager() = default;
    ~E_ResManager()
    {
        clear();
    }

    std::unordered_map<std::string, SDL_Texture *> texture_cache;
    std::unordered_map<std::string, E_Font *> font_cache;
};