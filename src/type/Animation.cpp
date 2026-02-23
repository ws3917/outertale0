#include "Animation.hpp"

#include "../engine/Global.hpp"
#include "../engine/ImageMgr.hpp"

Animation::Animation(const std::string& src, float interval)
    : Image(src), interval(interval) {
  frame_count = G::image->getFrame(src);
}

void Animation::setInterval(float interval) { this->interval = interval; }
void Animation::update(float delta) {
  if (!running) return;
  passed_time += delta;
  while (passed_time >= interval) {
    passed_time -= interval;
    current_frame++;
    current_frame %= frame_count;
  }
}
void Animation::draw() {
  G::image->draw(src, x, y, alpha, current_frame, scale);
}
void Animation::pause() { running = false; }
void Animation::resume() { running = true; }