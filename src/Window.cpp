//
// Created by l4zy9uy on 3/31/24.
//

#include "Application.h"

unsigned int Application::getScrWidth() const {
  return SCR_WIDTH;
}
unsigned int Application::getScrHeight() const {
  return SCR_HEIGHT;
}
GLFWwindow *Application::getWindow() const {
  return window;
}
void Application::setWindow(GLFWwindow *Window) {
  window = Window;
}
bool Application::isInitSuccess1() const {
  return isInitSuccess;
}
void Application::setIsInitSuccess(bool IsInitSuccess) {
  isInitSuccess = IsInitSuccess;
}