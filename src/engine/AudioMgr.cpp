#include "AudioMgr.hpp"

#include <SDL3_mixer/SDL_mixer.h>

#include <array>

#include "GlobalVars.hpp"

AudioMgr::AudioMgr() {
  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (!mixer) {
    SDL_Log("[C] <AudioMgr> Failed to initialize mixer: %s", SDL_GetError());
    G::status = SDL_APP_FAILURE;
    return;
  }
  // 创建音轨
  const std::array<AudioType, 3> types = {AudioType::Music, AudioType::Sound,
                                          AudioType::Voice};
  for (auto type : types) {
    auto& track = tracks[static_cast<size_t>(type)];
    track = {MIX_CreateTrack(mixer), SDL_CreateProperties()};
    if (!track.first) {
      SDL_Log("[C] <AudioMgr> Can't create track: %s", SDL_GetError());
      G::status = SDL_APP_FAILURE;
      return;
    }
    SDL_SetNumberProperty(track.second, MIX_PROP_PLAY_LOOPS_NUMBER,
                          type == AudioType::Music ? -1 : 0);
  }
}

AudioMgr::~AudioMgr() {
  MIX_StopAllTracks(mixer, 0);
  for (auto& track : tracks) {
    if (track.first) MIX_SetTrackAudio(track.first, nullptr);
  }
  for (auto& [_, obj] : audio_assets) MIX_DestroyAudio(obj.first);
  audio_assets.clear();
  for (auto& track : tracks) {
    MIX_DestroyTrack(track.first);
    track.first = nullptr;
    SDL_DestroyProperties(track.second);
  }
  if (mixer) {
    MIX_DestroyMixer(mixer);
    mixer = nullptr;
  }
}

void AudioMgr::play(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr> Can't find audio: %s", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  MIX_Audio* audio = obj.first;
  auto& track = tracks[(int)obj.second];
  MIX_SetTrackAudio(track.first, audio);
  MIX_PlayTrack(track.first, track.second);
}

void AudioMgr::stop(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr> Can't find audio: %s", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  MIX_Audio* audio = obj.first;
  auto& track = tracks[(int)obj.second];
  MIX_SetTrackAudio(nullptr, audio);
  MIX_StopTrack(track.first, track.second);
}

bool AudioMgr::load(const std::string& name, AudioType type,
                    const std::string& path) {
  if (audio_assets.count(name)) return true;

  MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(),
                                   type == AudioType::Music ? false : true);

  if (!audio) {
    SDL_Log("[E] <AudioMgr> Can't load sound '%s': %s", path.c_str(),
            SDL_GetError());
    return false;
  }
  audio_assets[name] = {audio, type};
  return true;
}