#include "third-party/gl.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "src/Vertex.h"
#include "src/Shader.h"
#include "src/Window.h"
#include "src/Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/Camera.h"
//--------------------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void init();
void clear(GLuint &VAO, GLuint &VBO, GLuint &EBO);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
Camera camera({0.0f, 0.0f, 3.0f});


int main() {
  init();
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL",
                                        nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window,static_cast<GLFWcursorposfun>(mouse_callback));
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (!gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD\n";
    return -1;
  }
  glEnable(GL_DEPTH_TEST);

  Shader ourShader("../src/shader.vs", "../src/shader.fs");
  std::vector<Vertex> vertices{
      {{0.5f, 0.5f, 0.5f},    {1, 1, 1}, {1.0f, 1.0f}},
      {{0.5f, -0.5f, 0.5f},   {1, 1, 1}, {1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f},  {0, 0, 0}, {0, 0}},
      {{-0.5f, 0.5f, 0.5f},   {0, 0, 0}, {0.0f, 1.0f}},
      {{0.5f, 0.5f, -0.5f},   {1, 1, 1}, {1.0f, 1.0f}},
      {{0.5f, -0.5f, -0.5f},  {1, 1, 1}, {1.0f, 0.0f}},
      {{-0.5f, -0.5f, -0.5f}, {0, 0, 0}, {0, 0}},
      {{-0.5f, 0.5f, -0.5f},  {0, 0, 0}, {0.0f, 1.0f}}
  };
  std::vector<glm::ivec3> indices{{2, 1, 0},
                                  {2, 3, 0},
                                  {3, 7, 6},
                                  {6, 2, 3},
                                  {6, 5, 4},
                                  {4, 7, 6},
                                  {0, 4, 5},
                                  {5, 1, 0},
                                  {6, 5, 1},
                                  {1, 2, 6},
                                  {7, 4, 0},
                                  {0, 3, 7}};

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
               vertices.data(),
               GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(glm::ivec3)),
               indices.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        glm::vec3::length(),
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,
                        glm::vec3::length(),
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void *) (sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, glm::vec2::length(), GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *) (2 * sizeof(glm::vec3)));
  glEnableVertexAttribArray(2);

  Texture texture1("../cat2.jpg");
  Texture texture2("../awesomeface.png");
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);
  auto thirdParam = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    auto currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      thirdParam += 0.1;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      thirdParam -= 0.1;
    ourShader.setFloat("thirdParam", thirdParam);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    texture1.bind(0);
    texture2.bind(1);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // camera/view transformation
    glm::mat4 view = camera.getViewMatrix();

    ourShader.setMatrix4x4("model", model);
    ourShader.setMatrix4x4("view", view);
    ourShader.setMatrix4x4("projection", projection);
    ourShader.use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size() * glm::ivec3::length(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ourShader.clear();
  clear(VAO, VBO, EBO);

}

void processInput(GLFWwindow *window) {
  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
void clear(GLuint &VAO, GLuint &VBO, GLuint &EBO) {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
  if (firstMouse)
  {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }
  float xoffset = xPos - lastX;
  float yoffset = lastY - yPos;
  lastX = xPos;
  lastY = yPos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}