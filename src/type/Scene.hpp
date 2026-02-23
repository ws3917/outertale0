#pragma once
#include <SDL3/SDL.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "Object.hpp"

// Scene 接口，一切场景需实现此接口
class Scene {
 public:
  Scene() = default;
  virtual ~Scene();
  virtual void enter() {}
  virtual void exit() {}
  virtual void update(float delta);
  virtual void draw();
  virtual void input(SDL_Event* event);
 protected:
  std::unordered_map<std::string, Object*> objects = {};
  std::vector<Object*> render_queue;
};