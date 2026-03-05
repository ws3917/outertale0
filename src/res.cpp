#include "res.hpp"

#include "type/audio.hpp"
#include "type/font.hpp"
#include "type/texture.hpp"

Res::Res() = default;
Res::~Res() = default;
void Res::init(SDL_Renderer* new_renderer, MIX_Mixer* new_mixer)
{
  renderer = new_renderer;
  mixer = new_mixer;
}
Audio& Res::getSound(const std::string& id) {
  auto result = sound_list.find(id);
  if (result != sound_list.end()) return *result->second;
  auto sound = std::make_unique<Audio>(mixer);
  sound->load("sound/" + id + ".mp3");
  auto [new_it, inserted] = sound_list.emplace(id, std::move(sound));
  return *new_it->second;
}
Audio& Res::getMusic(const std::string& id) {
  auto result = music_list.find(id);
  if (result != music_list.end()) return *result->second;
  auto music = std::make_unique<Audio>(mixer);
  music->load("music/" + id + ".mp3");
  auto [new_it, inserted] = music_list.emplace(id, std::move(music));
  return *new_it->second;
}
Texture& Res::getTexture(const std::string& id) {
  auto result = texture_list.find(id);
  if (result != texture_list.end()) return *result->second;
  auto texture = std::make_unique<Texture>(renderer);
  texture->load("image/" + id + ".png");
  auto [new_it, inserted] = texture_list.emplace(id, std::move(texture));
  return *new_it->second;
}
Font& Res::getFont(const std::string& id) {
  auto result = font_list.find(id);
  if (result != font_list.end()) return *result->second;
  auto font = std::make_unique<Font>(renderer);
  font->load("font/" + id + ".fnt");
  auto [new_it, inserted] = font_list.emplace(id, std::move(font));
  return *new_it->second;
}