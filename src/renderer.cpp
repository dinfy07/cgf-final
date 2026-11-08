#include "renderer.hpp"
Renderer::Renderer(const std::string &root)
    : bodyShader(root + "/shaders/body.vert", root + "/shaders/body.frag") {
}
void Renderer::draw(int w, int h) {
  glViewport(0, 0, w, h);
  glEnable(GL_DEPTH_TEST);
  glClearColor(.006f, .009f, .018f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto v = glm::lookAt(glm::vec3(0, 40, 80), glm::vec3(0), glm::vec3(0, 1, 0));
  auto p = projection(w, h);
  bodyShader.use();
  bodyShader.set("view", v);
  bodyShader.set("projection", p);
}
