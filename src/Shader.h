//
// Created by l4zy9uy on 4/4/24.
//

#ifndef GPU_RAYTRACER_SRC_SHADER_H
#define GPU_RAYTRACER_SRC_SHADER_H

#include <string>
#include <GLFW/glfw3.h>
class Shader {
private:
  GLuint id_;
public:
  // constructor reads and builds the shader
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  [[nodiscard]] const GLuint & getId() const;
  void setId(const GLuint &Id);
  // use/activate the shader
  void use() const;
  void clear();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
};

#endif //GPU_RAYTRACER_SRC_SHADER_H
