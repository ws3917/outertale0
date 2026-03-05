#include "audio.hpp"

#include <SDL3_mixer/SDL_mixer.h>

void Audio::MIX_AudioDeleter::operator()(MIX_Audio* audio) const {
  if (audio) MIX_DestroyAudio(audio);
}
bool Audio::load(const std::string& filepath) {
  if (!Object::load(filepath)) return false;
  loaded = false;
  audio.reset();
  SDL_SeekIO(io.get(), 0, SDL_IO_SEEK_SET);
  audio.reset(MIX_LoadAudio_IO(mixer, io.get(), true, false));
  if (!audio) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load audio: %s", SDL_GetError());
    return false;
  }
  loaded = true;
  return true;
}
void Audio::play() {
  if (!loaded || !audio) return;
  MIX_SetTrackAudio(track, audio.get());
  MIX_PlayTrack(track, 0);
}
void Audio::stop() {
  if (!loaded || !audio) return;
  MIX_SetTrackAudio(track, audio.get());
  MIX_StopTrack(track, 0);
}