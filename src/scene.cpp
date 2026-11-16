#include "scene.hpp"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

const std::array<Body, 9> Scene::bodies{
    {{"Mercury", "mercury", "A rocky inner world", .38f, 5.0f, 88.f, 58.6f,
      .03f, .3f},
     {"Venus", "venus_atmosphere", "A world wrapped in clouds", .65f, 7.7f,
      224.7f, -243.f, 177.4f, 1.6f},
     {"Earth", "earth_daymap", "Explore the day and night boundary", .72f, 11.f,
      365.25f, 1.f, 23.4f, 2.6f},
     {"Mars", "mars", "The red planet", .50f, 14.3f, 687.f, 1.03f, 25.2f, 4.1f},
     {"Jupiter", "jupiter", "A gas giant with banded clouds", 1.65f, 21.f,
      4333.f, .414f, 3.1f, 5.2f},
     {"Saturn", "saturn", "Ice and dust form its ring system", 1.35f, 28.f,
      10759.f, .444f, 26.7f, .9f},
     {"Uranus", "uranus", "An ice giant tilted on its side", .98f, 35.f,
      30687.f, -.718f, 97.8f, 2.9f},
     {"Neptune", "neptune", "The outermost planet", .95f, 42.f, 60190.f, .671f,
      28.3f, 4.7f},
     {"Moon", "moon", "Earth is the parent of this orbital frame", .20f, 2.05f,
      27.3f, 27.3f, 6.7f, 0.f}}};

glm::mat4 Scene::frame(int i) const {
  const auto &b = bodies.at(i);
  float angle =
      float(std::fmod(days / b.period, 1.) * glm::two_pi<double>()) + b.phase;
  auto orbit = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0));
  auto local = orbit * glm::translate(glm::mat4(1), glm::vec3(b.orbit, 0, 0));

  // A translation-only Earth anchor prevents Earth's spin/scale from affecting
  // the Moon.
  return i == 8 ? glm::translate(glm::mat4(1), position(2)) * local : local;
}

glm::vec3 Scene::position(int i) const {
  return glm::vec3(frame(i) * glm::vec4(0, 0, 0, 1));
}

glm::mat4 Scene::model(int i) const {
  const auto &b = bodies.at(i);
  auto m = glm::translate(glm::mat4(1), position(i));
  m = glm::rotate(m, glm::radians(b.tilt), glm::vec3(0, 0, 1));
  m = glm::rotate(m,
                  float(std::fmod(days / b.spin, 1.) * glm::two_pi<double>()),
                  glm::vec3(0, 1, 0));
  return glm::scale(m, glm::vec3(b.radius));
}
float Scene::focusDistance(int i) const {
  return i < 0
             ? 70.f
             : (i == 8 ? 2.8f
                       : (i == 5 ? 8.f
                                 : std::max(3.5f, bodies.at(i).radius * 5.f)));
}
