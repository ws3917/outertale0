#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

class Texture;
class Audio;
class Font;
class Res {
 public:
  Res(SDL_Renderer* renderer, MIX_Mixer* mixer);
  ~Res();
  Texture& getTexture(const std::string& id);
  Audio& getMusic(const std::string& id);
  Audio& getSound(const std::string& id);
  Font& getFont(const std::string& id);

 private:
  SDL_Renderer* renderer;
  MIX_Mixer* mixer;
  std::unordered_map<std::string, std::unique_ptr<Texture>> texture_list = {};
  std::unordered_map<std::string, std::unique_ptr<Audio>> sound_list = {};
  std::unordered_map<std::string, std::unique_ptr<Audio>> music_list = {};
  std::unordered_map<std::string, std::unique_ptr<Font>> font_list = {};
};