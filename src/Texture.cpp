//
// Created by l4zy9uy on 4/7/24.
//


#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"
#include "Texture.h"
#include "Vertex.h"
#include <iostream>
#include <fstream>
#include <vector>
Texture::Texture(const std::string &filePath) {
  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);
// set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  auto data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

  if (data != nullptr) {
    int format;
    if (isPNG(filePath))
      format = GL_RGBA;
    else
      format = GL_RGB;
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 width,
                 height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture\n";
  }
  stbi_image_free(data);
}
const GLuint &Texture::getId() const {
  return id_;
}
void Texture::setId(const GLuint &Id) {
  id_ = Id;
}
void Texture::bind(const GLuint &textureUnit) const {
  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glBindTexture(GL_TEXTURE_2D, id_);
}
Texture::~Texture() {
  //glDeleteTextures(1, &id_);
}
bool Texture::isPNG(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Unable to open file " << filename << "\n";
    return false;
  }

  std::vector<char> signature(8);
  file.read(signature.data(), 8);

  return signature
      == std::vector<char>{'\x89', 'P', 'N', 'G', '\r', '\n', '\x1a', '\n'};

}
