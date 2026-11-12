#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <random>
#include <stdexcept>
#include <vector>
Texture::Texture(const std::string &path) {
  int w, h, c;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path.c_str(), &w, &h, &c, 3);
  if (!data)
    throw std::runtime_error("Cannot load texture: " + path);
  glGenTextures(1, &id);
  glBindTexture(target, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(target, 0, GL_SRGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  glGenerateMipmap(target);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
Texture::Texture() : target(GL_TEXTURE_CUBE_MAP) {
  glGenTextures(1, &id);
  glBindTexture(target, id);
  std::mt19937 rng(2148);
  constexpr int size = 512;
  for (int face = 0; face < 6; ++face) {
    std::vector<unsigned char> pixels(size * size * 3, 2);
    for (int i = 0; i < 800; ++i) {
      int x = 2 + rng() % (size - 4), y = 2 + rng() % (size - 4),
          b = 90 + rng() % 166;
      auto p = (y * size + x) * 3;
      pixels[p] = b * .85f;
      pixels[p + 1] = b * .92f;
      pixels[p + 2] = b;
      if (i % 20 == 0)
        for (int d : {-1, 1}) {
          pixels[p + d * 3] = b / 3;
          pixels[p + d * size * 3 + 2] = b / 3;
        }
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB8, size, size,
                 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  }
  glGenerateMipmap(target);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  for (auto axis : {GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R})
    glTexParameteri(target, axis, GL_CLAMP_TO_EDGE);
}
