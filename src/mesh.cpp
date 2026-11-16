#include "mesh.hpp"
#include <cstddef>
#include <glm/gtc/constants.hpp>
#include <stdexcept>

Geometry sphereGeometry(int longitude, int latitude) {
  if (longitude < 3 || latitude < 2)
    throw std::invalid_argument("Sphere resolution too small");

  Geometry g;
  for (int y = 0; y <= latitude; ++y)
    for (int x = 0; x <= longitude; ++x) {
      float u = float(x) / longitude, v = float(y) / latitude;
      float p = u * glm::two_pi<float>(), t = v * glm::pi<float>();
      glm::vec3 n{sin(t) * cos(p), cos(t), sin(t) * sin(p)};
      g.vertices.push_back({n, n, {u, 1 - v}});
    }

  for (int y = 0; y < latitude; ++y)
    for (int x = 0; x < longitude; ++x) {
      unsigned a = y * (longitude + 1) + x, b = a + longitude + 1;
      if (y != 0)
        g.indices.insert(g.indices.end(), {a, a + 1, b});
      if (y != latitude - 1)
        g.indices.insert(g.indices.end(), {a + 1, b + 1, b});
    }

  return g;
}

Geometry ringGeometry(int segments) {
  if (segments < 3)
    throw std::invalid_argument("Ring resolution too small");
  Geometry g;
  for (int i = 0; i <= segments; ++i)
    for (int j = 0; j < 2; ++j) {
      float a = float(i) / segments * glm::two_pi<float>(),
            r = j ? 2.35f : 1.35f;
      g.vertices.push_back({{r * cos(a), 0, r * sin(a)},
                            {0, 1, 0},
                            {float(j), float(i) / segments}});
    }
  for (int i = 0; i < segments; ++i) {
    unsigned a = i * 2;
    g.indices.insert(g.indices.end(), {a, a + 2, a + 1, a + 1, a + 2, a + 3});
  }
  return g;
}
Mesh::Mesh(const Geometry &g) : count(GLsizei(g.indices.size())) {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(g.vertices.size() * sizeof(Vertex)),
               g.vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               GLsizeiptr(g.indices.size() * sizeof(unsigned)),
               g.indices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));
  glBindVertexArray(0);
}

void Mesh::setInstances(const std::vector<glm::mat4> &transforms) {
  if (!instances)
    glGenBuffers(1, &instances);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, instances);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(transforms.size() * sizeof(glm::mat4)),
               transforms.data(), GL_STATIC_DRAW);
  for (int i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(sizeof(glm::vec4) * i));
    glVertexAttribDivisor(3 + i, 1);
  }
  glBindVertexArray(0);
}
void Mesh::draw(GLenum mode, int instanceCount) const {
  glBindVertexArray(vao);
  if (instanceCount > 0)
    glDrawElementsInstanced(mode, count, GL_UNSIGNED_INT, nullptr,
                            instanceCount);
  else
    glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
}
Mesh::~Mesh() {
  glDeleteBuffers(1, &instances);
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}
