#include "renderer.hpp"
Renderer::Renderer(const std::string &root)
    : bodyShader(root + "/shaders/body.vert", root + "/shaders/body.frag"), sphere(sphereGeometry()),
      skyShader(root + "/shaders/sky.vert", root + "/shaders/sky.frag") {
  for (int i = 0; i < 9; ++i)
    textures[i] = std::make_unique<Texture>(root + "/assets/" + Scene::bodies[i].texture + ".jpg");
  sun = std::make_unique<Texture>(root + "/assets/sun.jpg");
  night = std::make_unique<Texture>(root + "/assets/earth_nightmap.jpg");
}
void Renderer::draw(const Scene &s, const Camera &c, int w, int h) {
  glViewport(0, 0, w, h);
  glEnable(GL_DEPTH_TEST);
  glClearColor(.006f, .009f, .018f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto v = c.view(), p = projection(w, h);
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  skyShader.use();
  skyShader.set("view", v);
  skyShader.set("projection", p);
  skyShader.set("sky", 0);
  sky.bind();
  sphere.draw();
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
  bodyShader.use();
  bodyShader.set("view", v);
  bodyShader.set("projection", p);
  bodyShader.set("dayTexture", 0);
  bodyShader.set("cameraPosition", c.position);
  bodyShader.set("nightTexture", 1);
  bodyShader.set("headlight", int(s.headlight));
  bodyShader.set("lightPower", s.lightPower);
  bodyShader.set("nightBlend", int(s.night));
  bodyShader.set("textured", 1);
  bodyShader.set("earth", 0);
  bodyShader.set("emissive", 1);
  night->bind(1);
  bodyShader.set("model", glm::scale(glm::mat4(1), glm::vec3(2.f)));
  sun->bind();
  sphere.draw();
  bodyShader.set("emissive", 0);
  for (int i = 0; i < 9; ++i) {
    bodyShader.set("model", s.model(i));
    textures[i]->bind();
    bodyShader.set("earth", int(i == 2));
    sphere.draw();
  }
}
