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

    Scene scene;
    double last = glfwGetTime();
    {
      Renderer renderer(SOLAR_SOURCE_DIR);
      while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime(), rawDt = now - last;
        last = now;
        glfwPollEvents();
        scene.advance(rawDt);
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        if (fw == 0 || fh == 0) {
          glfwWaitEventsTimeout(.1);
          continue;
        }
        renderer.draw(scene, fw, fh);
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
