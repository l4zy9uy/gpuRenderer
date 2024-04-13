//
// Created by l4zy9uy on 4/13/24.
//

#ifndef GPU_RAYTRACER_SRC_CAMERA_H
#define GPU_RAYTRACER_SRC_CAMERA_H

#include "../third-party/gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// Default camera values
const float Yaw = -90.0f;
const float Pitch = 0.0f;
const float Speed = 2.5f;
const float Sensitivity = 0.1f;
const float Zoom = 45.0f;

class Camera {
public:
  // camera Attributes
  glm::vec3 position{};
  glm::vec3 front{};
  glm::vec3 up{};
  glm::vec3 right{};
  glm::vec3 worldUp{};
  // euler Angles
  float yaw{};
  float pitch{};
  // camera options
  float movementSpeed{};
  float mouseSensitivity{};
  float zoom{};
public:
  Camera();
  explicit Camera(const glm::vec3 &position_ = {},
                  const glm::vec3 &up_ = {0.0f, 1.0f, 0.0f},
                  const float &yaw_ = Yaw,
                  const float &pitch_ = Pitch);
public:
  [[nodiscard]] glm::mat4 getViewMatrix() const;
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  void ProcessMouseMovement(float xOffset,
                            float yOffset,
                            GLboolean constrainPitch = true);
private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void updateCameraVectors();
};
#endif //GPU_RAYTRACER_SRC_CAMERA_H
