#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "renderer.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "ui.hpp"

struct App {
  Scene scene;
  Camera camera;
  UI ui;
  double mx = 0, my = 0;
  bool dragging = false;
};

static App *state(GLFWwindow *w) {
  return static_cast<App *>(glfwGetWindowUserPointer(w));
}

static void key(GLFWwindow *w, int k, int, int action, int) {
  if (action != GLFW_PRESS || ImGui::GetIO().WantCaptureKeyboard)
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
  if (k == GLFW_KEY_H)
    a.ui.visible = !a.ui.visible;
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
  if (!ImGui::GetIO().WantCaptureMouse)
    state(w)->camera.zoom(float(y));
}

static void screenshot(const fs::path &path, int w, int h) {
  std::vector<unsigned char> pixels(size_t(w) * h * 3);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  std::ofstream out(path, std::ios::binary);
  out << "P6\n" << w << " " << h << "\n255\n";
  for (int y = h - 1; y >= 0; --y)
    out.write(reinterpret_cast<char *>(pixels.data() + size_t(y) * w * 3),
              w * 3);
  if (!out)
    throw std::runtime_error("Failed to save screenshot: " + path.string());
}

static void tour(App &a, double t) {
  auto &s = a.scene;
  auto &c = a.camera;
  int step = int(t / 20);
  s.selected = -1;
  s.speed = 2;
  s.night = true;
  s.rings = true;
  s.asteroids = true;
  s.headlight = false;
  s.paused = false;
  c.free = false;
  const char *captions[] = {
      "R1 / R2 / R3 - C++ and OpenGL core. Eight textured planets, a star, a moon and a parent-child orbit hierarchy.",
      "R5 - Equirectangular planet maps with UV coordinates, trilinear mipmapping and a six-face star cubemap.",
      "Stretch 1 - Earth blends day and city-light textures using the signed dot product of its normal and sunlight direction.",
      "R4 - Per-fragment Blinn-Phong shading. The Sun lights the day side. The switchable camera light is the second source.",
      "Stretch 2 - Saturn has transparent, banded rings. Opaque objects write depth before alpha blending the ring.",
      "Stretch 3 - 2,200 asteroids share one mesh and one instanced draw call. Static instance matrices upload once.",
      "R6 - Travel to any planet with keys 1-8. Mouse and arrow keys orbit. Scroll changes distance. F enables free flight.",
      "R6 - Pause, resume and change time speed. Orbital motion uses elapsed seconds, independently of frame rate.",
      "R3 - Follow the Moon. Its world transform is composed from Earth's translation and its own local orbit.",
      "R7 / R8 - The live FPS counter stays visible. README documents the CMake build. Stylized scale, not an ephemeris."};
  a.ui.caption = captions[std::min(step, 9)];
  if (step == 1)
    s.selected = 4;
  if (step == 2 || step == 3) {
    s.selected = 2;
    s.speed = .12f;
    if (step == 3)
      s.headlight = int(t / 5) % 2 == 0;
  }
  if (step == 4)
    s.selected = 5;
  if (step == 5) {
    s.selected = -1;
    c.distance = 38;
  }
  if (step == 6)
    s.selected = std::min(7, int(t - 120) / 3);
  if (step == 7) {
    s.selected = 2;
    s.speed = 12;
    s.paused = int(t / 5) % 2 == 0;
  }
  if (step == 8)
    s.selected = 8;
  if (step == 9)
    s.selected = 2;
  if (step != 5)
    c.distance = s.focusDistance(s.selected);
  c.yaw += .045f / 60.f;
}

int main(int argc, char **argv) {
  int width = 1280, height = 720;
  double seconds = 0;
  bool touring = false, hidden = false, vsync = true;
  std::string capture, stats, root = SOLAR_SOURCE_DIR;
  try {
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      auto value = [&]() {
        if (++i >= argc)
          throw std::runtime_error("Missing value for " + arg);
        return std::string(argv[i]);
      };
      if (arg == "--width")
        width = std::stoi(value());
      else if (arg == "--height")
        height = std::stoi(value());
      else if (arg == "--seconds")
        seconds = std::stod(value());
      else if (arg == "--capture")
        capture = value();
      else if (arg == "--stats")
        stats = value();
      else if (arg == "--assets-root")
        root = value();
      else if (arg == "--tour")
        touring = true;
      else if (arg == "--hidden")
        hidden = true;
      else if (arg == "--no-vsync")
        vsync = false;
      else if (arg == "--help") {
        std::cout << "solar_explorer [--tour] [--seconds N] [--width W --height H] [--capture DIR] [--stats FILE] [--assets-root DIR] [--no-vsync] [--hidden]\n";
        return 0;
      } else
        throw std::runtime_error("Unknown option: " + arg);
    }
    if (width < 640 || height < 480 || seconds < 0)
      throw std::runtime_error("Invalid dimensions or duration");
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
    glfwWindowHint(GLFW_VISIBLE, hidden ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    // One framebuffer pixel per window point makes 1280x720 benchmarks
    // unambiguous on Retina.
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    auto *window = glfwCreateWindow(width, height, "Solar System Explorer",
                                    nullptr, nullptr);
    if (!window)
      throw std::runtime_error("Cannot create OpenGL window");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
      throw std::runtime_error("Cannot load OpenGL");
    std::cout << "OpenGL: " << glGetString(GL_VERSION)
              << "\nRenderer: " << glGetString(GL_RENDERER) << '\n';

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetIO().FontGlobalScale = 1.f;
    UI::style();

    App app;
    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, key);
    glfwSetScrollCallback(window, scroll);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    if (!capture.empty())
      fs::create_directories(capture);
    std::ofstream samples;
    if (!stats.empty()) {
      samples.open(stats);
      samples << "elapsed_seconds,frame_ms,width,height,view\n";
    }
    unsigned frames = 0, errors = 0;
    double begin = glfwGetTime(), last = begin, stamp = begin;
    int lastShot = -1;
    float fps = 60;
    {
      Renderer renderer(root);
      while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime(), elapsed = now - begin, rawDt = now - last;
        last = now;
        if (seconds > 0 && elapsed >= seconds)
          break;
        float dt = float(std::min(rawDt, .10));
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (touring)
          tour(app, elapsed);
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        bool drag =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
            !ImGui::GetIO().WantCaptureMouse;
        if (drag && app.dragging)
          app.camera.rotate(float(x - app.mx), float(y - app.my));
        app.mx = x;
        app.my = y;
        app.dragging = drag;
        auto down = [&](int k) {
          return glfwGetKey(window, k) == GLFW_PRESS ? 1.f : 0.f;
        };
        if (!ImGui::GetIO().WantCaptureKeyboard) {
          app.camera.yaw += (down(GLFW_KEY_LEFT) - down(GLFW_KEY_RIGHT)) * dt;
          app.camera.pitch = std::clamp(
              app.camera.pitch + (down(GLFW_KEY_UP) - down(GLFW_KEY_DOWN)) * dt,
              -1.45f, 1.45f);
          if (app.camera.free)
            app.camera.move(down(GLFW_KEY_W) - down(GLFW_KEY_S),
                            down(GLFW_KEY_D) - down(GLFW_KEY_A),
                            down(GLFW_KEY_E) - down(GLFW_KEY_Q), dt);
        }
        app.scene.advance(rawDt);
        app.camera.update(app.scene.selected < 0
                              ? glm::vec3(0)
                              : app.scene.position(app.scene.selected),
                          dt);
        int fw, fh;
        glfwGetFramebufferSize(window, &fw, &fh);
        if (fw == 0 || fh == 0) {
          ImGui::EndFrame();
          glfwWaitEventsTimeout(.1);
          continue;
        }
        renderer.draw(app.scene, app.camera, fw, fh);
        int ww, wh;
        glfwGetWindowSize(window, &ww, &wh);
        app.ui.draw(app.scene, app.camera, ww, wh, fps);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (!capture.empty()) {
          int shot = int(elapsed / 20);
          if (elapsed - shot * 20 >= 8 && shot != lastShot) {
            screenshot(fs::path(capture) / ("view_" + std::to_string(shot) + ".ppm"), fw, fh);
            lastShot = shot;
          }
        }
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
          ++errors;
          std::cerr << "GL error: " << error << '\n';
        }
        if (samples && elapsed > 2)
          samples << std::fixed << std::setprecision(5) << elapsed << ','
                  << rawDt * 1000 << ',' << fw << ',' << fh << ','
                  << app.scene.selected << '\n';
        glfwSwapBuffers(window);
        ++frames;
        if (now - stamp >= .5) {
          fps = float(frames / (now - stamp));
          frames = 0;
          stamp = now;
          std::string title =
              "Solar System Explorer | " + std::to_string(int(fps)) + " FPS";
          glfwSetWindowTitle(window, title.c_str());
        }
      }
    }
    std::cout << "OpenGL errors: " << errors << '\n';
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return errors ? 2 : 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    glfwTerminate();
    return 1;
  }
}
