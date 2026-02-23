// 资源管理器
#pragma once
#include <miniaudio.h>

#include <string>
#include <unordered_map>

class AudioMgr {
 public:
  AudioMgr(ma_engine* mixer);
  ~AudioMgr();
  bool load(const std::string& name, const std::string& type,
            const std::string& path);
  bool loads(const std::string& config_path);
  void play(const std::string& name);
  void stop(const std::string& name);
  enum class AudioType { Music, Sound, Voice };

 private:
  const std::unordered_map<std::string, AudioType> type_map = {
      {"music", AudioType::Music},
      {"sound", AudioType::Sound},
      {"voice", AudioType::Voice}};
  struct AudioProperty {
    ma_sound* audio;
    ma_decoder* decoder;
    void* data;
    AudioType type;
  };
  ma_engine* mixer = nullptr;
  std::unordered_map<std::string, AudioProperty> audio_assets = {};
};