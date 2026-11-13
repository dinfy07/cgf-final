#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "renderer.hpp"
#include "scene.hpp"
#include "camera.hpp"

struct App {
  Scene scene;
  Camera camera;
  double mx = 0, my = 0;
  bool dragging = false;
};

static App *state(GLFWwindow *w) {
  return static_cast<App *>(glfwGetWindowUserPointer(w));
}

static void key(GLFWwindow *w, int k, int, int action, int) {
  if (action != GLFW_PRESS)
    return;
  auto &a = *state(w);
  auto &s = a.scene;
  if (k == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(w, true);
  if (k >= GLFW_KEY_1 && k <= GLFW_KEY_9) {
    s.selected = k - GLFW_KEY_1;
    a.camera.free = false;
    a.camera.distance = s.focusDistance(s.selected);
  }
  if (k == GLFW_KEY_M) {
    s.selected = 8;
    a.camera.free = false;
    a.camera.distance = s.focusDistance(8);
  }
  if (k == GLFW_KEY_0) {
    s.selected = -1;
    a.camera.free = false;
    a.camera.distance = 70;
  }
  if (k == GLFW_KEY_SPACE)
    s.paused = !s.paused;
  if (k == GLFW_KEY_L)
    s.headlight = !s.headlight;
  if (k == GLFW_KEY_N)
    s.night = !s.night;
  if (k == GLFW_KEY_G)
    s.rings = !s.rings;
  if (k == GLFW_KEY_B)
    s.asteroids = !s.asteroids;
  if (k == GLFW_KEY_O)
    s.orbits = !s.orbits;
  if (k == GLFW_KEY_F)
    a.camera.free = !a.camera.free;
  if (k == GLFW_KEY_V)
    s.wireframe = !s.wireframe;
  if (k == GLFW_KEY_R) {
    s.reset();
    a.camera = Camera();
  }
  if (k == GLFW_KEY_EQUAL || k == GLFW_KEY_KP_ADD)
    s.speed = std::min(120.f, s.speed + 3);
  if (k == GLFW_KEY_MINUS || k == GLFW_KEY_KP_SUBTRACT)
    s.speed = std::max(0.f, s.speed - 3);
}

static void scroll(GLFWwindow *w, double, double y) {
  state(w)->camera.zoom(float(y));
}

int main() {
  int width = 1280, height = 720;
  try {
    glfwSetErrorCallback([](int e, const char *m) {
      std::cerr << "GLFW " << e << ": " << m << '\n';
    });
    if (!glfwInit())
      throw std::runtime_error("GLFW initialization failed");
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    // One framebuffer pixel per window point makes 1280x720 benchmarks
    // unambiguous on Retina.
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    auto *window = glfwCreateWindow(width, height, "Solar System Explorer",
                                    nullptr, nullptr);
    if (!window)
      throw std::runtime_error("Cannot create OpenGL window");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
      throw std::runtime_error("Cannot load OpenGL");
    std::cout << "OpenGL: " << glGetString(GL_VERSION)
              << "\nRenderer: " << glGetString(GL_RENDERER) << '\n';

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    App app;
    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, key);
    glfwSetScrollCallback(window, scroll);
    double last = glfwGetTime();
    {
      Renderer renderer(SOLAR_SOURCE_DIR);
      while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime(), rawDt = now - last;
        last = now;
        float dt = float(std::min(rawDt, .10));
        glfwPollEvents();
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        bool drag =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if (drag && app.dragging)
          app.camera.rotate(float(x - app.mx), float(y - app.my));
        app.mx = x;
        app.my = y;
        app.dragging = drag;
        auto down = [&](int k) {
          return glfwGetKey(window, k) == GLFW_PRESS ? 1.f : 0.f;
        };
        app.camera.yaw += (down(GLFW_KEY_LEFT) - down(GLFW_KEY_RIGHT)) * dt;
        app.camera.pitch = std::clamp(
            app.camera.pitch + (down(GLFW_KEY_UP) - down(GLFW_KEY_DOWN)) * dt,
            -1.45f, 1.45f);
        if (app.camera.free)
          app.camera.move(down(GLFW_KEY_W) - down(GLFW_KEY_S),
                          down(GLFW_KEY_D) - down(GLFW_KEY_A),
                          down(GLFW_KEY_E) - down(GLFW_KEY_Q), dt);
        app.scene.advance(rawDt);
        app.camera.update(app.scene.selected < 0
                              ? glm::vec3(0)
                              : app.scene.position(app.scene.selected),
                          dt);
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        if (fw == 0 || fh == 0) {
          glfwWaitEventsTimeout(.1);
          continue;
        }
        renderer.draw(app.scene, app.camera, fw, fh);
        glfwSwapBuffers(window);
      }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    glfwTerminate();
    return 1;
  }
}