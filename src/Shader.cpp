//
// Created by l4zy9uy on 4/4/24.
//

#include "Shader.h"
#include "../third-party/gl.h"
#include <iostream>
#include <fstream>
#include <sstream>

const GLuint &Shader::getId() const {
  return id_;
}

void Shader::setId(const GLuint &id) {
  id_ = id;
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  std::string vertexCode, fragmentCode;
  std::ifstream vShaderFile, fShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  auto vShaderCode = vertexCode.c_str();
  auto fShaderCode = fragmentCode.c_str();
  int success;
  char infoLog[512];

  auto vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, nullptr);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
  }
  auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
  }

  // shader Program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id_, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() const {
  glUseProgram(id_);
}

void Shader::setBool(const std::string &name, const bool &value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, const int &value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float &value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::clear() const {
  glDeleteProgram(id_);
}

void Shader::setMatrix4x4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()),
                     1,
                     GL_FALSE,
                     &mat[0][0]);
}
void Shader::setVector4f(const std::string &name, const glm::vec4 &vec) const {
  glUniform4f(glGetUniformLocation(id_, name.c_str()),
              vec.x, vec.y, vec.z, vec.w);
}
void Shader::setVector3f(const std::string &name, const glm::vec3 &vec) const {
  glUniform3f(glGetUniformLocation(id_, name.c_str()),
              vec.x, vec.y, vec.z);
}
void Shader::setVector2f(const std::string &name, const glm::vec2 &vec) const {
  glUniform2f(glGetUniformLocation(id_, name.c_str()),
              vec.x, vec.y);
}
