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
//--------------------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void init();
void clear(GLuint &VAO, GLuint &VBO, GLuint &EBO);

int main()
{
  init();
  // glfw window creation
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL",
                                        nullptr, nullptr);
  if (window == nullptr)
  {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGL((GLADloadfunc )glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  Shader ourShader("../src/shader.vs", "../src/shader.fs");
  std::vector<Vertex> vertices {
      {{0.5f, 0.5f, 0.0f}, {1, 1, 1}, {1.0f, 1.0f}},
      {{0.5f, -0.5f, 0.0f}, {1, 1, 1}, {1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.0f}, {0, 0, 0}, {0, 0}},
      {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}
  };
  std::vector<glm::ivec3> indices {{2, 1, 0},
                                   {2, 3, 0},
                                   {2, 1, 0},
                                   {2, 3, 0}};

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(glm::ivec3)), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)(sizeof(glm::vec3)));//reinterpret_cast<void*>(sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
  glEnableVertexAttribArray(2);

  Texture texture1("../cat2.jpg");
  Texture texture2("../awesomeface.png");
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);
  auto thirdParam = 0.0f;
  while(!glfwWindowShouldClose(window)) {
    processInput(window);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      thirdParam += 0.1;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      thirdParam -= 0.1;
    ourShader.setFloat("thirdParam", thirdParam);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 transform{1.0f}; // make sure to initialize matrix to identity matrix first
    transform = glm::rotate(transform, static_cast<float>(glfwGetTime()), {0.0f, 0.0f, 1.0f});
    transform = glm::translate(transform, {0.5f, -0.5f, 0.0f});

    // get matrix's uniform location and set matrix
    ourShader.use();
    auto transformLoc = glGetUniformLocation(ourShader.getId(), "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    texture1.bind(0);
    texture2.bind(1);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_INT, nullptr);
    transform = glm::mat4(1.0f); // reset it to identity matrix
    transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
    auto scaleAmount = static_cast<float>(sin(glfwGetTime()));
    transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value
    glDrawElements(GL_TRIANGLES, 6,  GL_UNSIGNED_INT, nullptr);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ourShader.clear();
  clear(VAO, VBO, EBO);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void init() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}
void clear(GLuint &VAO, GLuint &VBO, GLuint &EBO) {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  //glDeleteShader(shaderProgram);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
}