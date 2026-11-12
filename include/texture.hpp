#pragma once
#include <glad/gl.h>
#include <string>
class Texture {
  GLuint id = 0;
  GLenum target = GL_TEXTURE_2D;

public:
  explicit Texture(const std::string &path);
  Texture(); // Deterministic six-face procedural star cubemap.
  ~Texture() {
    if (id)
      glDeleteTextures(1, &id);
  }
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  void bind(int unit = 0) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, id);
  }
};
