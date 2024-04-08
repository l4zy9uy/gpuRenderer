//
// Created by l4zy9uy on 4/7/24.
//
#ifndef GPU_RAYTRACER_SRC_TEXTURE_H
#define GPU_RAYTRACER_SRC_TEXTURE_H
#include <string>
#include "../third-party/gl.h"
#include <GLFW/glfw3.h>

class Texture {
private:
  GLuint id_{};
public:
  [[nodiscard]] const GLuint & getId() const;
  void setId(const GLuint &Id);
  void bind(const GLuint &textureUnit) const;
public:
  Texture(const std::string &filePath);
  ~Texture();
private:
  static bool isPNG(const std::string& filename);
};

#endif //GPU_RAYTRACER_SRC_TEXTURE_H
