#pragma once
#include <SDL3/SDL.h>

// Object 接口，一切对象需实现此接口
class Object {
 public:
  Object() = default;
  virtual ~Object() = default;
  virtual void update(float) {};
  virtual void draw() {};
  virtual void input(SDL_Event*) {};
  void setActive(bool active) { this->active = active; }
  float x = 0.0;
  float y = 0.0;
  float w = 0.0;
  float h = 0.0;
  // 控制渲染层，数字越小越靠底层
  int layer = 0;

 private:
  bool active = true;
};