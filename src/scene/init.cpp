#include "init.hpp"

#include "../type/audio.hpp"
#include "../type/texture.hpp"

SInit::SInit(Res* res) : Scene(res) {
  for (int i = 0; i < 7; i++) {
    story_panel.push_back(
        &res->getTexture("init/story_panel_" + std::to_string(i)));
  }
  dst_panel_rect = {170.0f, 40.0f, story_panel[0]->getSize().x,
                    story_panel[0]->getSize().y};
  objects.insert(objects.end(), story_panel.begin(), story_panel.end());
}

void SInit::update(float dt) {
  if (current_panel >= story_panel.size()) return;
  state_timer += dt;
  switch (state) {
    case PanelState::FadeIn: {
      float progress = state_timer / FADE_DUR;
      if (progress >= 1.0f) {
        state = PanelState::Showing;
        panel_alpha = 255;
        state_timer = 0.0f;
        break;
      } else {
        panel_alpha = static_cast<uint8_t>(progress * 255);
      }
      break;
    }
    case PanelState::Showing:
      if (state_timer >= 4.5f) {
        state_timer = 0.0;
        state = PanelState::FadeOut;
      }
      break;
    case PanelState::FadeOut: {
      float progress = state_timer / FADE_DUR;
      if (progress >= 1.0f) {
        current_panel++;
        state = PanelState::FadeIn;
        panel_alpha = 0;
        state_timer = 0.0f;

        if (current_panel >= story_panel.size()) {
          // todo: 幻灯片播放完成，进入 Press Enter
        }
        break;
      } else {
        panel_alpha = static_cast<uint8_t>((1.0f - progress) * 255.0f);
      }
      break;
    }
  }
}
void SInit::enter() { res->getMusic("intro").play(); }
void SInit::input(const SDL_Event& event) {}
void SInit::draw() {
  if (current_panel >= 0 && current_panel < story_panel.size()) {
    story_panel[current_panel]->drawEx(nullptr, &dst_panel_rect, panel_alpha);
  }
}
void SInit::exit() { res->getMusic("intro").stop(); }