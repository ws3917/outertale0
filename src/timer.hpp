#pragma once
#include <functional>
class Timer {
 public:
  using Callback = std::function<void()>;
  Timer(float duration, bool loop, Callback callback = {});
  void start();
  void pause();
  void reset();
  void update(float dt);
  void setDuration(float duration);
  void setCallback(Callback callback);
  bool isActive();

 private:
  float duration;
  bool loop = false;
  bool active = false;
  Callback callback;
  float remaining;
};