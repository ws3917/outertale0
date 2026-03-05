#pragma once
#include "scene.hpp"
class SInit : public Scene {
 public:
  using Scene::Scene;
  void update(float dt) override;
  void input(const SDL_Event& event) override;
  void draw() override;

 private:
};