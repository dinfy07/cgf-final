#include "renderer.hpp"

Renderer::Renderer(const std::string &root)
    : bodyShader(root + "/shaders/body.vert", root + "/shaders/body.frag"), sphere(sphereGeometry()) {
}

void Renderer::draw(int w, int h) {
  glViewport(0, 0, w, h);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  auto p = projection(w, h);
  auto v = glm::mat4(1.0f);
  v[3][2] = -5.0f; 

  bodyShader.use();
  bodyShader.set("view", v);
  bodyShader.set("projection", p);
  bodyShader.set("model", glm::scale(glm::mat4(1.0f), glm::vec3(2.f)));
  bodyShader.set("tint", glm::vec3(1.f, .75f, .25f));
  sphere.draw();
}