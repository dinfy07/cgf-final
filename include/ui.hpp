#pragma once
#include "camera.hpp"
#include "scene.hpp"
#include <string>
struct UI {
  bool help = true, visible = true;
  std::string caption;
  void draw(Scene &s, Camera &c, int width, int height, float fps);
  static void style();
};
