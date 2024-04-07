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
  GLuint getId() const;
  void setId(GLuint Id);
public:
  Texture(const std::string &filePath);
};

#endif //GPU_RAYTRACER_SRC_TEXTURE_H
