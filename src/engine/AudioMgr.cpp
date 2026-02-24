#include "AudioMgr.hpp"

#include <yyjson.h>

#include "Utils.hpp"

AudioMgr::AudioMgr(ma_engine* mixer) : mixer(mixer) {}
bool AudioMgr::loads(const std::string& config_path) {
  size_t size;
  yyjson_doc* config = Utils::LoadJson(config_path);
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
  return true;
}

AudioMgr::~AudioMgr() {
  for (auto& [_, obj] : audio_assets) {
    ma_decoder_uninit(obj.decoder);
    ma_sound_uninit(obj.audio);
    if (obj.data) SDL_free(obj.data);
    delete obj.decoder;
    delete obj.audio;
  }
  audio_assets.clear();
}

void AudioMgr::play(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr - play> Can't find audio '%s'.", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  if (obj.type != AudioType::Music) ma_sound_seek_to_pcm_frame(obj.audio, 0);
  ma_sound_start(obj.audio);
}

void AudioMgr::stop(const std::string& name) {
  if (!audio_assets.count(name)) {
    SDL_Log("[E] <AudioMgr - stop> Can't find audio '%s'.", name.c_str());
    return;
  }
  auto& [_, obj] = *audio_assets.find(name);
  ma_sound_stop(obj.audio);
}

bool AudioMgr::load(const std::string& name, const std::string& type,
                    const std::string& path) {
  if (audio_assets.count(name)) return true;
  size_t data_size;
  void* data = Utils::LoadFile(path, &data_size);
  if (!data) {
    SDL_Log("[E] <AudioMgr - load> Sound file is not found: %s", path.c_str());
    return false;
  }
  ma_decoder* decoder = new ma_decoder;
  if (ma_decoder_init_memory(data, data_size, nullptr, decoder) != MA_SUCCESS) {
    SDL_Log("[E] <AudioMgr - load> Can't decode audio from memory.");
    SDL_free(data);
    return false;
  }
  AudioType real_type =
      type_map.count(type) ? type_map.at(type) : AudioType::Sound;
  int flag = real_type == AudioType::Music
                 ? MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_LOOPING
                 : 0;
  ma_sound* audio = new ma_sound;
  if (ma_sound_init_from_data_source(mixer, decoder, flag, nullptr, audio) !=
      MA_SUCCESS) {
    SDL_Log("[E] <AudioMgr - load> Can't load sound '%s'.", path.c_str());
    return false;
  }
  audio_assets[name] = {audio, decoder, data, real_type};
  return true;
}