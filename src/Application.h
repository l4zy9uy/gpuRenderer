//
// Created by l4zy9uy on 3/31/24.
//

#ifndef GPU_RAYTRACER_SRC_APPLICATION_H
#define GPU_RAYTRACER_SRC_APPLICATION_H

#include <GLFW/glfw3.h>
class Application {
private:
  // settings
  const unsigned int SCR_WIDTH = 800;
  const unsigned int SCR_HEIGHT = 600;
public:
  unsigned int getScrWidth() const;
  unsigned int getScrHeight() const;
  GLFWwindow *getWindow() const;
  void setWindow(GLFWwindow *Window);
  bool isInitSuccess1() const;
  void setIsInitSuccess(bool IsInitSuccess);
private:
  GLFWwindow* window{};
  bool isInitSuccess{};
public:
  void init();
  void terminate();
};


#endif //GPU_RAYTRACER_SRC_APPLICATION_H
