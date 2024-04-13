//
// Created by l4zy9uy on 4/13/24.
//

#include "Camera.h"
Camera::Camera() {
  position = {0.0f, 0.0f, 0.0f};
  worldUp = {0.0f, 1.0f, 0.0f};
  yaw = Yaw;
  pitch = Pitch;
  updateCameraVectors();
}
Camera::Camera(const glm::vec3 &position_,
               const glm::vec3 &up_,
               const float &yaw_,
               const float &pitch_) : front({0.0f, 0.0f, -1.0f}),
                                      movementSpeed(Speed),
                                      mouseSensitivity(Sensitivity),
                                      zoom(Zoom) {
  position = position_;
  worldUp = up_;
  yaw = yaw_;
  pitch = pitch_;
  updateCameraVectors();
}
glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);

}
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (direction == FORWARD)
    position += front * velocity;
  if (direction == BACKWARD)
    position -= front * velocity;
  if (direction == LEFT)
    position -= right * velocity;
  if (direction == RIGHT)
    position += right * velocity;
}
void Camera::ProcessMouseMovement(float xOffset,
                                  float yOffset,
                                  GLboolean constrainPitch) {
  xOffset *= mouseSensitivity;
  yOffset *= mouseSensitivity;

  yaw += xOffset;
  pitch += yOffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}
void Camera::updateCameraVectors() {
// calculate the new tmp_front vector
  glm::vec3 tmp_front {
      cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)),
      sinf(glm::radians(pitch)),
      sinf(glm::radians(yaw)) * cosf(glm::radians(pitch))
  };
  front = glm::normalize(tmp_front);
  right = glm::normalize(glm::cross(tmp_front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  up    = glm::normalize(glm::cross(right, tmp_front));
}
