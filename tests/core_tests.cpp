#include "camera.hpp"
#include "scene.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

void require(bool v, const char *m) {
  if (!v)
    throw std::runtime_error(m);
}

int main() {
  try {
    Scene s;
    for (double t : {0., 1., 27.3, 365.25, 10000., -200.}) {
      s.days = t;
      auto earth = s.position(2), moon = s.position(8);
      require(std::abs(glm::length(moon - earth) - Scene::bodies[8].orbit) <
                  1e-4,
              "Moon must remain at its local orbit radius from Earth");
      for (int i = 0; i < 8; ++i)
        require(std::abs(glm::length(s.position(i)) - Scene::bodies[i].orbit) <
                    1e-4,
                "Planet orbit radius invariant");
      for (int i = 0; i < 9; ++i) {
        auto p = s.model(i) * glm::vec4(0, 0, 0, 1);
        require(glm::length(glm::vec3(p) - s.position(i)) < 1e-4,
                "Local spin must not move center");
      }
    }
    Scene a, b;
    for (int i = 0; i < 60; ++i)
      a.advance(1. / 60);
    for (int i = 0; i < 120; ++i)
      b.advance(1. / 120);
    require(std::abs(a.days - b.days) < 1e-9,
            "Time evolution must be frame-rate independent");
    a.paused = true;
    auto old = a.days;
    a.advance(10);
    require(a.days == old, "Pause must stop simulation");
    Camera c;
    c.zoom(1e4);
    require(c.distance >= 1, "Minimum camera distance");
    c.zoom(-1e4);
    require(c.distance <= 240, "Maximum camera distance");
    c.rotate(0, 1e5);
    require(std::abs(c.pitch) <= 1.45001f, "Camera pitch clamp");
    Camera x, y;
    x.update({4, 0, 0}, .1f);
    y.update({4, 0, 0}, .05f);
    y.update({4, 0, 0}, .05f);
    require(glm::length(x.target - y.target) < 1e-5,
            "Camera smoothing must compose over time");
    std::cout << "PASS: hierarchy, centers, orbit radii, time step, pause, "
                 "camera bounds and smoothing\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "FAIL: " << e.what() << '\n';
    return 1;
  }
}
