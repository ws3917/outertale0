#pragma once
#include <SDL3/SDL.h>
// Scene 接口，一切场景需实现此接口
class E_Scene
{
  public:
    E_Scene() = default;
    virtual ~E_Scene() = default;
    virtual void enter() {}
    virtual void exit() {}
    virtual void update(float delta) = 0;
    virtual void draw(SDL_Renderer *renderer) = 0;
    virtual void input(SDL_Event *event) = 0;

    int id;
};