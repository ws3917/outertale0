#pragma once
#include "../engine/E_Scene.hpp"

class S_Init : public E_Scene
{
  public:
    void enter() override;
    void update(float delta) override;
    void draw(SDL_Renderer *renderer) override;
    void input(SDL_Event *event) override;
};