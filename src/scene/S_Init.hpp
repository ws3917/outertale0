#pragma once
#include "../type/Scene.hpp"

class S_Init : public Scene {
 public:
  S_Init();
  void enter() override;
};