#pragma once
#include "scene.hpp"

class SInit : public Scene {
 public:
  SInit(Res* res);
  void update(float dt) override;
  void input(const SDL_Event& event) override;
  void draw() override;
  void enter() override;
  void exit() override;

 private:
  enum class PanelState { FadeIn, Showing, FadeOut };
  std::vector<Texture*> story_panel;
  int current_panel = 0;
  int panel_alpha = 0;
  PanelState state = PanelState::FadeIn;
  float state_timer = 0.0f;
  const float FADE_DUR = 0.8f;
  SDL_FRect dst_panel_rect;
};