//
// Created by l4zy9uy on 4/7/24.
//


#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"
#include "Texture.h"
#include "Vertex.h"
#include <iostream>
Texture::Texture(const std::string &filePath) {
  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);
// set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
  glEnableVertexAttribArray(2);
// load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  auto data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

  if (data != nullptr)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture\n";
  }
  stbi_image_free(data);
}
GLuint Texture::getId() const {
  return id_;
}
void Texture::setId(GLuint Id) {
  id_ = Id;
}
