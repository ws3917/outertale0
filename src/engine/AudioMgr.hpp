// 资源管理器
#pragma once
#include <memory>
#include <miniaudio/miniaudio.h>
#include <string>
#include <unordered_map>

class ImageMgr
{
  public:
    enum class AudioType
    {
        Music,
        Sound
    };
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
    bool load(AudioType type, const std::string &name, const std::string &path);

    ma_sound *find(const std::string &name);
    // loop = true 代表循环播放，resume = true代表继续播放
    void play(const std::string &name, bool loop = false, bool resume = false);
    void stop(const std::string &name);
    void clear();

  private:
    ImageMgr() = default;
    ~ImageMgr();

    ma_engine engine;
    std::unordered_map<std::string, std::unique_ptr<ma_sound>> images;
};