#pragma once
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera {
public:
  glm::vec3 target{0}, position{0, 40, 80};
  float yaw = 0.78f, pitch = 0.53f, distance = 70.f;
  bool free = false;
  glm::vec3 direction() const {
    return {-cos(pitch) * cos(yaw), -sin(pitch), -cos(pitch) * sin(yaw)};
  }
  void update(glm::vec3 desired, float dt) {
    if (!free) {
      target = glm::mix(target, desired, 1.f - std::exp(-7.f * dt));
      position = target - direction() * distance;
    }
  }
  void rotate(float x, float y) {
    yaw -= x * .005f;
    pitch = std::clamp(pitch + y * .005f, -1.45f, 1.45f);
  }
  void zoom(float amount) {
    distance = std::clamp(distance * std::exp(-amount * .12f), 1.f, 240.f);
  }
  glm::mat4 view() const {
    return glm::lookAt(position, position + direction(), {0, 1, 0});
  }
  void move(float forward, float right, float up, float dt) {
    auto d = direction(), r = glm::normalize(glm::cross(d, glm::vec3(0, 1, 0)));
    position += (d * forward + r * right + glm::vec3(0, up, 0)) * dt *
                std::max(3.f, distance * .5f);
  }
};
