#include "Timer.hpp"

Timer::Timer(float interval, bool oneshot)
    : interval(interval), oneshot(oneshot) {}

void Timer::resume() { running = true; }
void Timer::pause() { running = false; }

void Timer::reset() {
  triggered = false;
  passed_time = 0.0f;
}

void Timer::setInterval(float interval) { this->interval = interval; }
void Timer::setOneShot(bool oneshot) { this->oneshot = oneshot; }
void Timer::setCallback(std::function<void()> callback) {
  this->callback = callback;
}

void Timer::update(float delta) {
  if (!running) return;
  passed_time += delta;

  if (passed_time >= interval) {
    // 单次触发
    if (oneshot) {
      if (!triggered) {
        triggered = true;
        pause();
        if (callback) callback();
      }
    } else {
      // 循环模式
      while (passed_time >= interval) passed_time -= interval;
      if (callback) callback();
    }
  }
}