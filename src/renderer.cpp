#include "renderer.hpp"
#include <glm/gtc/constants.hpp>
#include <random>
Renderer::Renderer(const std::string &root)
    : bodyShader(root + "/shaders/body.vert", root + "/shaders/body.frag"),
      skyShader(root + "/shaders/sky.vert", root + "/shaders/sky.frag"),
      sphere(sphereGeometry()), ring(ringGeometry()),
      asteroid(sphereGeometry(8, 5)) {
  for (int i = 0; i < 9; ++i)
    textures[i] = std::make_unique<Texture>(root + "/assets/" +
                                            Scene::bodies[i].texture + ".jpg");
  night = std::make_unique<Texture>(root + "/assets/earth_nightmap.jpg");
  sun = std::make_unique<Texture>(root + "/assets/sun.jpg");
  std::mt19937 rng(2148);
  std::uniform_real_distribution<float> u(0, 1);
  std::vector<glm::mat4> transforms;
  transforms.reserve(asteroidCount);
  for (int i = 0; i < asteroidCount; ++i) {
    float a = u(rng) * glm::two_pi<float>(), r = 16.5f + 2.6f * u(rng),
          s = .035f + .08f * u(rng);
    auto m = glm::translate(
        glm::mat4(1), glm::vec3(cos(a) * r, (u(rng) - .5f) * .8f, sin(a) * r));
    m = glm::rotate(
        m, u(rng) * 6.28f,
        glm::normalize(glm::vec3(u(rng) + .1f, u(rng) + .1f, u(rng) + .1f)));
    transforms.push_back(glm::scale(m, glm::vec3(s, s * .75f, s * 1.2f)));
  }
  asteroid.setInstances(transforms);
  std::vector<glm::vec3> points;
  for (int i = 0; i < 256; ++i) {
    float a = float(i) / 256 * glm::two_pi<float>();
    points.push_back({cos(a), 0, sin(a)});
  }
  glGenVertexArrays(1, &orbitVAO);
  glGenBuffers(1, &orbitVBO);
  glBindVertexArray(orbitVAO);
  glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(points.size() * sizeof(glm::vec3)),
               points.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
}
Renderer::~Renderer() {
  glDeleteBuffers(1, &orbitVBO);
  glDeleteVertexArrays(1, &orbitVAO);
}
void Renderer::draw(const Scene &s, const Camera &c, int w, int h) {
  glViewport(0, 0, w, h);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
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
  bodyShader.set("cameraPosition", c.position);
  bodyShader.set("dayTexture", 0);
  bodyShader.set("nightTexture", 1);
  bodyShader.set("headlight", int(s.headlight));
  bodyShader.set("lightPower", s.lightPower);
  bodyShader.set("nightBlend", int(s.night));
  bodyShader.set("instanced", 0);
  bodyShader.set("ring", 0);
  bodyShader.set("textured", 1);
  bodyShader.set("earth", 0);
  bodyShader.set("emissive", 1);
  night->bind(1);
  sun->bind();
  bodyShader.set("model", glm::scale(glm::mat4(1), glm::vec3(2.f)));
  sphere.draw();
  bodyShader.set("emissive", 0);
  if (s.wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (int i = 0; i < 9; ++i) {
    bodyShader.set("model", s.model(i));
    bodyShader.set("earth", int(i == 2));
    textures[i]->bind();
    sphere.draw();
  }
  bodyShader.set("earth", 0);
  bodyShader.set("textured", 0);
  if (s.asteroids) {
    bodyShader.set("instanced", 1);
    bodyShader.set("tint", glm::vec3(.20f, .17f, .14f));
    bodyShader.set("model",
                   glm::rotate(glm::mat4(1),
                               float(std::fmod(s.days / 1600., 1.) * 6.28),
                               glm::vec3(0, 1, 0)));
    asteroid.draw(GL_TRIANGLES, asteroidCount);
    bodyShader.set("instanced", 0);
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  if (s.orbits) {
    bodyShader.set("emissive", 1);
    bodyShader.set("tint", glm::vec3(.020f, .035f, .055f));
    glBindVertexArray(orbitVAO);
    glVertexAttrib3f(1, 0, 1, 0);
    glVertexAttrib2f(2, 0, 0);
    for (int i = 0; i < 9; ++i) {
      auto m =
          i == 8 ? glm::translate(glm::mat4(1), s.position(2)) : glm::mat4(1);
      bodyShader.set("model", glm::scale(m, glm::vec3(Scene::bodies[i].orbit)));
      glDrawArrays(GL_LINE_LOOP, 0, 256);
    }
    bodyShader.set("emissive", 0);
  }
  if (s.rings) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    bodyShader.set("ring", 1);
    bodyShader.set("tint", glm::vec3(.72f, .60f, .40f));
    auto m = glm::translate(glm::mat4(1), s.position(5));
    m = glm::rotate(m, glm::radians(26.7f), glm::vec3(0, 0, 1));
    bodyShader.set("model", glm::scale(m, glm::vec3(Scene::bodies[5].radius)));
    ring.draw();
    bodyShader.set("ring", 0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
}
