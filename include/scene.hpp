#pragma once
#include <array>
#include <glm/glm.hpp>
#include <string>
struct Body {
  const char *name, *texture, *description;
  float radius, orbit, period, spin, tilt, phase;
};
class Scene {
public:
  static const std::array<Body, 9> bodies;
  double days = 23;
  float speed = 12.f;
  bool paused = false;
  bool headlight = false, night = true;
  bool rings = true, asteroids = true, orbits = true, wireframe = false;
  float lightPower = 3.f;
  int selected = -1;
  void advance(double dt) {
    if (!paused)
      days += dt * speed;
  }
  glm::mat4 frame(int index) const;
  glm::mat4 model(int index) const;
  glm::vec3 position(int index) const;
  float focusDistance(int index) const;
  void reset() { *this = Scene(); }
};
