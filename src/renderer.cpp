#include "renderer.hpp"
Renderer::Renderer(const std::string &root)
    : bodyShader(root + "/shaders/body.vert", root + "/shaders/body.frag"), sphere(sphereGeometry()) {
}
void Renderer::draw(const Scene &s, int w, int h) {
  glViewport(0, 0, w, h);
  glEnable(GL_DEPTH_TEST);
  glClearColor(.006f, .009f, .018f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto v = glm::lookAt(glm::vec3(0, 40, 80), glm::vec3(0), glm::vec3(0, 1, 0));
  auto p = projection(w, h);
  bodyShader.use();
  bodyShader.set("view", v);
  bodyShader.set("projection", p);
  bodyShader.set("model", glm::scale(glm::mat4(1), glm::vec3(2.f)));
  bodyShader.set("tint", glm::vec3(1.f, .75f, .25f));
  sphere.draw();
  for (int i = 0; i < 9; ++i) {
    bodyShader.set("model", s.model(i));
    bodyShader.set("tint", glm::vec3(.25f + .07f * i, .6f, .9f - .06f * i));
    sphere.draw();
  }
}
