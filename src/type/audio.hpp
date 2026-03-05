#pragma once
#include "object.hpp"
#include <SDL3_mixer/SDL_mixer.h>
class Audio : public Object {
 public:
  Audio(MIX_Mixer* mixer, MIX_Track* track) : mixer(mixer), track(track) {}
  bool load(const std::string& filepath) override;
  void play();
  void stop();

 private:
  struct MIX_AudioDeleter {
    void operator()(MIX_Audio* audio) const;
  };
  MIX_Mixer* mixer = nullptr;
  MIX_Track* track = nullptr;
  std::unique_ptr<MIX_Audio, MIX_AudioDeleter> audio;
};