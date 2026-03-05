#include "timer.hpp"
Timer::Timer(float duration, bool loop, Callback callback)
    : duration(duration), loop(loop), callback(callback), remaining(duration) {}
void Timer::start() { active = true; }
void Timer::pause() { active = false; }
void Timer::reset() {
  active = false;
  remaining = duration;
}

void Timer::update(float dt) {
  if (!active || duration <= 0) return;
  remaining -= dt;
  if (remaining <= 0) {
    if (loop)
      while (remaining < 0) {
        remaining += duration;
        if (callback) callback();
      }
    else {
      active = false;
      remaining = 0;
      if (callback) callback();
    }
  }
}
void Timer::setCallback(Callback new_callback) {
  callback = new_callback;
}
void Timer::setDuration(float new_duration) { duration = new_duration; }
bool Timer::isActive() { return active; }