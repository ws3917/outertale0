#pragma once
#include "../engine/Global.hpp"
#include "../engine/ImageMgr.hpp"
#include "Object.hpp"

class Image : public Object {
 public:
  Image(const std::string& src) : src(src) {}
  void draw() override { G::image->draw(src, x, y, alpha, 0, scale); };
  float scale = 1.0;
  float alpha = 1.0;

 protected:
  std::string src;
};