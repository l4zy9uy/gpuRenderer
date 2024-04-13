//
// Created by l4zy9uy on 4/4/24.
//

#ifndef GPU_RAYTRACER_SRC_SHADER_H
#define GPU_RAYTRACER_SRC_SHADER_H

#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader {
private:
  GLuint id_;
public:
  // constructor reads and builds the shader
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  [[nodiscard]] const GLuint &getId() const;
  void setId(const GLuint &Id);
  // use/activate the shader
  void use() const;
  void clear() const;
  // utility uniform functions
  void setBool(const std::string &name, const bool &value) const;
  void setInt(const std::string &name, const int &value) const;
  void setFloat(const std::string &name, const float &value) const;
  void setMatrix4x4(const std::string &name, const glm::mat4 &mat) const;
  void setVector4f(const std::string &name, const glm::vec4 &vec) const;
  void setVector3f(const std::string &name, const glm::vec3 &vec) const;
  void setVector2f(const std::string &name, const glm::vec2  &vec) const;
};

#endif //GPU_RAYTRACER_SRC_SHADER_H
