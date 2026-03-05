#pragma once
#include <SDL3/SDL.h>

#include "../type/object.hpp"
#include "../res.hpp"

// 这是一个纯虚基类（接口）
class Scene {
 public:
  Scene(Res* res) : res{res} {}
  virtual ~Scene() = default;
  virtual void input(const SDL_Event& event) = 0;
  virtual void update(float dt) = 0;
  virtual void draw() = 0;
  virtual void enter() = 0;
  virtual void exit() = 0;

 protected:
  std::vector<Object*> objects;
  Res* res;
};