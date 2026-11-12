#pragma once
#include "shader.hpp"
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
public:
  explicit Renderer(const std::string &root);
  void draw(const Scene &scene, const Camera &camera, int width, int height);
};