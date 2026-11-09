#pragma once
#include "shader.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
#include "scene.hpp"
class Renderer {
  Shader bodyShader;
  Mesh sphere;
public:
  explicit Renderer(const std::string &root);
  void draw(const Scene &scene, int width, int height);
  static glm::mat4 projection(int w, int h) {
    auto p = glm::perspective(glm::radians(48.f), float(w) / std::max(1, h),
                              .05f, 800.f);
    p[2][0] = -.23f;
    return p;
  }
};
