#pragma once
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <memory>
#include <string>
#include <unordered_map>


class Texture;
class Audio;
class Font;
class Res {
 public:
  Res();
  ~Res();
  void init(SDL_Renderer* renderer, MIX_Mixer* mixer);
  Texture& getTexture(const std::string& id);
  Audio& getMusic(const std::string& id);
  Audio& getSound(const std::string& id);
  Font& getFont(const std::string& id);

 private:
  struct MIX_AudioDeleter {
    void operator()(MIX_Track* track) const;
  };
  SDL_Renderer* renderer = nullptr;
  MIX_Mixer* mixer = nullptr;
  std::unique_ptr<MIX_Track, MIX_AudioDeleter> music_track;
  std::unique_ptr<MIX_Track, MIX_AudioDeleter> sound_track;
  std::unordered_map<std::string, std::unique_ptr<Texture>> texture_list = {};
  std::unordered_map<std::string, std::unique_ptr<Audio>> sound_list = {};
  std::unordered_map<std::string, std::unique_ptr<Audio>> music_list = {};
  std::unordered_map<std::string, std::unique_ptr<Font>> font_list = {};
};