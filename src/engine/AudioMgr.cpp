#include "AudioMgr.hpp"

#include <yyjson.h>

AudioMgr::AudioMgr(MIX_Mixer* mixer) : mixer(mixer) {}
bool AudioMgr::init() {
  // 创建音轨
  for (auto& [type, obj] : tracks) {
    obj.track = MIX_CreateTrack(mixer);
    if (!obj.track) {
      SDL_Log("[C] <AudioMgr> Can't create track: %s", SDL_GetError());
      return false;
    }
    SDL_SetNumberProperty(obj.options, MIX_PROP_PLAY_LOOPS_NUMBER,
                          type == "music" ? -1 : 0);
  }
  return true;
}
bool AudioMgr::loads(const std::string& config_path) {
  size_t size;
  void* config_data = SDL_LoadFile(config_path.c_str(), &size);
  if (!config_data) {
    SDL_Log("[E] <AudioMgr - loads> Can't open json file '%s': %s",
            config_path.c_str(), SDL_GetError());
    return false;
  }
  yyjson_read_err err;
  yyjson_doc* config =
      yyjson_read_opts((char*)config_data, size, 0, nullptr, &err);
  if (!config) {
    SDL_free(config_data);
    SDL_Log("[E] <AudioMgr - loads> Can't parse json file %s: %s",
            config_path.c_str(), err.msg);
    return false;
  }

  yyjson_val* sounds = yyjson_doc_ptr_get(config, "/audio");
  if (!sounds || !yyjson_is_arr(sounds)) {
    SDL_Log("[E] <AudioMgr - loads> No sounds found in %s",
            config_path.c_str());
    return false;
  }
  SDL_Log("[I] <AudioMgr - loads> Loading sounds in %s...",
          config_path.c_str());
  size_t idx, max;
  yyjson_val* sound;
  yyjson_arr_foreach(sounds, idx, max, sound) {
    if (!yyjson_is_obj(sound)) continue;
    const char* name = yyjson_get_str(yyjson_obj_get(sound, "name"));
    const char* path = yyjson_get_str(yyjson_obj_get(sound, "path"));
    const char* type = yyjson_get_str(yyjson_obj_get(sound, "type"));
    if (!name || !path) continue;
    if (load(name, type, std::string("audio/") + type + "/" + path))
      SDL_Log("[I] <AudioMgr - loads> Loaded sound '%s' (type: %s).", name,
              type);
    else
      SDL_Log("[E] <AudioMgr - loads> Failed to load sound '%s'.", name);
  }
  yyjson_doc_free(config);
  SDL_free(config_data);
  return true;
}
AudioMgr::~AudioMgr() {
  MIX_StopAllTracks(mixer, 0);
  for (auto& [_, obj] : tracks) {
    if (obj.track) MIX_SetTrackAudio(obj.track, nullptr);
  }
  for (auto& [_, obj] : audio_assets) MIX_DestroyAudio(obj.audio);
  audio_assets.clear();
  for (auto& [_, obj] : tracks) {
    MIX_DestroyTrack(obj.track);
    obj.track = nullptr;
    SDL_DestroyProperties(obj.options);
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
  MIX_SetTrackAudio(obj.track->track, obj.audio);
  MIX_PlayTrack(obj.track->track, obj.track->options);
}

void AudioMgr::stop(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr> Can't find audio: %s", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  MIX_Audio* audio = obj.audio;
  MIX_SetTrackAudio(nullptr, audio);
  MIX_StopTrack(obj.track->track, 0);
}

bool AudioMgr::load(const std::string& name, const std::string& type,
                    const std::string& path) {
  if (audio_assets.count(name)) return true;

  MIX_Audio* audio =
      MIX_LoadAudio(mixer, path.c_str(), type == "music" ? false : true);

  if (!audio) {
    SDL_Log("[E] <AudioMgr> Can't load sound '%s': %s", path.c_str(),
            SDL_GetError());
    return false;
  }
  TrackProperty* property = tracks.count(type) ? &tracks[type] : nullptr;
  audio_assets[name] = {audio, property};
  return true;
}