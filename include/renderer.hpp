#pragma once
#include "shader.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "mesh.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include <array>
#include <memory>
class Renderer {
  Shader bodyShader;
  Mesh sphere;
  Shader skyShader;
  Texture sky;
  std::array<std::unique_ptr<Texture>, 9> textures;
  std::unique_ptr<Texture> sun;
  std::unique_ptr<Texture> night;
public:
  explicit Renderer(const std::string &root);
  void draw(const Scene &scene, const Camera &camera, int width, int height);
  static glm::mat4 projection(int w, int h) {
    auto p = glm::perspective(glm::radians(48.f), float(w) / std::max(1, h),
                              .05f, 800.f);
    p[2][0] = -.23f;
    return p;
  }
};
