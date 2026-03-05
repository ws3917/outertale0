#pragma once
#include "object.hpp"
class MIX_Mixer;
class MIX_Audio;
class Audio : public Object {
 public:
  Audio(MIX_Mixer* mixer) : mixer(mixer) {}
  bool load(const std::string& filepath) override;
  void play();

 private:
  struct MIX_AudioDeleter {
    void operator()(MIX_Audio* audio) const;
  };
  MIX_Mixer* mixer = nullptr;
  std::unique_ptr<MIX_Audio, MIX_AudioDeleter> audio;
};