#pragma once
#include "camera.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <algorithm>
#include <array>
#include <memory>
class Renderer {
  Shader bodyShader, skyShader;
  Mesh sphere, ring, asteroid;
  Texture sky;
  std::array<std::unique_ptr<Texture>, 9> textures;
  std::unique_ptr<Texture> night, sun;
  GLuint orbitVAO = 0, orbitVBO = 0;
  static constexpr int asteroidCount = 2200;

public:
  explicit Renderer(const std::string &root);
  ~Renderer();
  void draw(const Scene &scene, const Camera &camera, int width, int height);
  static glm::mat4 projection(int w, int h) {
    auto p = glm::perspective(glm::radians(48.f), float(w) / std::max(1, h),
                              .05f, 800.f);
    p[2][0] = -.23f;
    return p;
  }
};
