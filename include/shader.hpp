#pragma once
#include <fstream>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

class Shader {
  GLuint id = 0;
  static GLuint compile(GLenum type, const std::string &path) {
    std::ifstream in(path);
    if (!in)
      throw std::runtime_error("Cannot open shader: " + path);
    std::ostringstream data;
    data << in.rdbuf();
    auto code = data.str();
    const char *p = code.c_str();
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &p, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      char log[4096];
      glGetShaderInfoLog(s, sizeof(log), nullptr, log);
      glDeleteShader(s);
      throw std::runtime_error(path + ": " + log);
    }
    return s;
  }

public:
  Shader(const std::string &vert, const std::string &frag) {
    GLuint v = compile(GL_VERTEX_SHADER, vert), f = 0;
    try {
      f = compile(GL_FRAGMENT_SHADER, frag);
    } catch (...) {
      glDeleteShader(v);
      throw;
    }
    id = glCreateProgram();
    glAttachShader(id, v);
    glAttachShader(id, f);
    glLinkProgram(id);
    glDeleteShader(v);
    glDeleteShader(f);
    GLint ok;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
      char log[4096];
      glGetProgramInfoLog(id, sizeof(log), nullptr, log);
      glDeleteProgram(id);
      id = 0;
      throw std::runtime_error(log);
    }
  }
  ~Shader() {
    if (id)
      glDeleteProgram(id);
  }
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;
  void use() const { glUseProgram(id); }
  void set(const char *n, int v) const {
    glUniform1i(glGetUniformLocation(id, n), v);
  }
  void set(const char *n, float v) const {
    glUniform1f(glGetUniformLocation(id, n), v);
  }
  void set(const char *n, const glm::vec3 &v) const {
    glUniform3fv(glGetUniformLocation(id, n), 1, glm::value_ptr(v));
  }
  void set(const char *n, const glm::mat4 &v) const {
    glUniformMatrix4fv(glGetUniformLocation(id, n), 1, GL_FALSE,
                       glm::value_ptr(v));
  }
};
