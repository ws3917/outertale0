#pragma once
class SDL_Event;
class S0 {
 public:
  void update(float dt);
  void draw();
  void input(SDL_Event* event);
 private:
};