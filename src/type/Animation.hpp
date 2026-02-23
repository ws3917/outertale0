#pragma once
#include <SDL3/SDL.h>

#include "Image.hpp"

class Animation : public Image {
 public:
  Animation(const std::string& src, float interval = 16.67f);
  void setInterval(float interval);
  void update(float delta) override;
  void draw() override;
  void pause();
  void resume();

 private:
  int frame_count = 0;
  int current_frame = 0;
  float passed_time = 0.0f;
  float interval = 16.67f;
  bool running = true;
};