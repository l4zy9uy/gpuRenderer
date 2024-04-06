//
// Created by l4zy9uy on 4/3/24.
//

#ifndef GPU_RAYTRACER_SRC_VERTEX_H
#define GPU_RAYTRACER_SRC_VERTEX_H
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texture;
};

#endif //GPU_RAYTRACER_SRC_VERTEX_H
