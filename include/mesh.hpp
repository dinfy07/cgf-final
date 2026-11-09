#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
struct Vertex {
  glm::vec3 position, normal;
  glm::vec2 uv;
};
struct Geometry {
  std::vector<Vertex> vertices;
  std::vector<unsigned> indices;
};
Geometry sphereGeometry(int longitude = 64, int latitude = 32);
class Mesh {
  GLuint vao = 0, vbo = 0, ebo = 0;
  GLsizei count = 0;

public:
  explicit Mesh(const Geometry &geometry);
  ~Mesh();
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  void draw(GLenum mode = GL_TRIANGLES) const;
};