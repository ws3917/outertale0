#define MINIAUDIO_IMPLEMENTATION
#include "AudioMgr.hpp"
#include <SDL3/SDL_log.h>

bool ImageMgr::init()
{
    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS)
    {
        SDL_Log("[C] <AudioMgr> Failed to initialize audio engine, error code = %d", result);
        return false;
    }
    return true;
}

ImageMgr::~ImageMgr()
{
    clear();
    ma_engine_uninit(&engine);
}

void ImageMgr::clear()
{
    for (auto &[_, sound] : images)
    {
        ma_sound_stop(sound.get());
        ma_sound_uninit(sound.get());
    }
    images.clear();
}

ma_sound *ImageMgr::find(const std::string &name)
{
    auto it = images.find(name);
    if (it == images.end())
    {
        SDL_Log("[E] <AudioMgr> Can't find sound: %s", name.c_str());
        return nullptr;
    }
    return it->second.get();
}

void ImageMgr::play(const std::string &name, bool loop, bool resume)
{
    auto sound = find(name);
    if (!sound)
        return;
    ma_sound_set_looping(sound, loop);
    if (!resume)
        ma_sound_seek_to_pcm_frame(sound, 0);
    ma_sound_start(sound);
}

void ImageMgr::stop(const std::string &name)
{
    auto sound = find(name);
    if (!sound)
        return;
    ma_sound_stop(sound);
}

bool ImageMgr::load(AudioType type, const std::string &name, const std::string &path)
{
    if (images.count(name))
        return true;

    ma_sound_flags flags = type == AudioType::Music ? MA_SOUND_FLAG_STREAM : MA_SOUND_FLAG_DECODE;

    auto sound = std::make_unique<ma_sound>();
    ma_result res = ma_sound_init_from_file(&engine, path.c_str(), flags, nullptr, nullptr, sound.get());
    if (res != MA_SUCCESS)
    {
        SDL_Log("[E] <AudioMgr> Can't load sound: %s", name.c_str());
        return false;
    }
    images.emplace(name, std::move(sound));
    return true;
}