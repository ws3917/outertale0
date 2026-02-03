// 资源管理器
#pragma once
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>

class ImageMgr
{
  public:
    // 单例
    static ImageMgr *get()
    {
        static ImageMgr instance;
        return &instance;
    }
    ImageMgr(const ImageMgr &) = delete;
    ImageMgr &operator=(const ImageMgr &) = delete;

    bool init();
    // 加载资源
    template <typename T> T *load(SDL_Renderer *renderer, const std::string &name, const std::string &path);
    template <typename T> T *get(const std::string &name);

    void clear()
    {
        for (auto &[name, tex] : images)
        {
            SDL_DestroyTexture(tex.first);
        }
        images.clear();
    }

  private:
    ImageMgr() = default;
    ~ImageMgr()
    {
        clear();
    }

    std::unordered_map<std::string, std::pair<SDL_Texture *, int>> images;
};