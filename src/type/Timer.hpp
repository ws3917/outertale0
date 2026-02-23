#pragma once
#include <functional>

#include "Object.hpp"

class Timer : public Object {
 public:
  Timer() = default;
  ~Timer() = default;
  Timer(float interval, bool oneshot = false);

  void resume();
  void pause();
  void reset();
  void update(float delta) override;

  // 设置回调
  void setCallback(std::function<void()> callback);
  void setInterval(float interval);
  void setOneShot(bool oneshot);
  bool isRunning() const { return running; }

 private:
  float interval = 10.0f;
  bool oneshot = false;
  bool running = false;
  bool triggered = false;
  float passed_time = 0.0f;
  std::function<void()> callback;
};