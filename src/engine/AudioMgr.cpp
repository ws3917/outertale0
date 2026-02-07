#include "AudioMgr.hpp"

bool AudioMgr::init() {
  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (!mixer) {
    SDL_Log("[C] <AudioMgr> Failed to initialize mixer: %s", SDL_GetError());
    return false;
  }
  // 创建音轨
  for (size_t i = 0; i < tracks.size(); i++) {
    auto& track = tracks[i];
    track = MIX_CreateTrack(mixer);
    if (!track) {
      SDL_Log("[C] <AudioMgr> Can't create track: %s", SDL_GetError());
      return false;
    }
    if (i == (int)AudioType::Music) MIX_SetTrackLoops(tracks[i], -1);
  }
  return true;
}

AudioMgr::~AudioMgr() {
  clear();
  for (auto& track : tracks) {
    MIX_DestroyTrack(track);
    track = nullptr;
  }
  if (mixer) {
    MIX_DestroyMixer(mixer);
    mixer = nullptr;
  }
}

void AudioMgr::clear() {
  MIX_StopAllTracks(mixer, 0);
  for (auto& track : tracks) {
    if (track) MIX_SetTrackAudio(track, nullptr);
  }
  for (auto& [_, obj] : audio_assets) MIX_DestroyAudio(obj.first);
  audio_assets.clear();
}

void AudioMgr::play(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr> Can't find audio: %s", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  MIX_Audio* audio = obj.first;
  MIX_Track* track = tracks[(int)obj.second];
  MIX_SetTrackAudio(track, audio);
  MIX_PlayTrack(track, 0);
}

void AudioMgr::stop(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr> Can't find audio: %s", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  MIX_Audio* audio = obj.first;
  MIX_Track* track = tracks[(int)obj.second];
  MIX_SetTrackAudio(nullptr, audio);
  MIX_StopTrack(track, 0);
}

bool AudioMgr::load(AudioType type, const std::string& name,
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