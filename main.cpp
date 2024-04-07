
#define STB_IMAGE_IMPLEMENTATION
#include "third-party/gl.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "src/Vertex.h"
#include <memory>
#include "src/Shader.h"
#include "src/Window.h"
#include "third-party/stb_image.h"
//--------------------------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void init();
void clear(GLuint &VAO, GLuint &VBO, GLuint &EBO);

int main()
{
  init();
  // glfw window creation
  // --------------------
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
  // ---------------------------------------
  if (!gladLoadGL((GLADloadfunc )glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  Shader ourShader("../src/shader.vs", "../src/shader.fs");
  std::vector<Vertex> vertices {
      {{0.5f, 0.5f, 0.0f}, {1, 0, 0}, {1.0f, 1.0f}},
      {{0.5f, -0.5f, 0.0f}, {0, 1, 0}, {1.0f, 0.0f}},
      {{-0.5f, -0.5f, 0.0f}, {0, 0, 1}, {0, 0}},
      {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
  };
  std::vector<glm::ivec3> indices {{2, 1, 0},
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

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));//reinterpret_cast<void*>(sizeof(glm::vec3)));
  glEnableVertexAttribArray(1);

  unsigned int texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
// set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
  glEnableVertexAttribArray(2);
// load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  auto data = stbi_load("../container.jpg", &width, &height, &nrChannels, 0);

  if (data != nullptr)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  unsigned int texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
// set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  data = stbi_load("../awesomeface.png", &width, &height, &nrChannels, 0);
  if (data != nullptr)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  //Texture texture1("../container.jpg");
  //Texture texture2("../awesomeface.png");
  ourShader.use(); // don't forget to activate the shader before setting uniforms!
  glUniform1i(glGetUniformLocation(ourShader.getId(), "texture1"), 0); // set it manually
  ourShader.setInt("texture2", 1); // or with shader class
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);//.getId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);//.getId());
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

    //ourShader.use(); // don't forget to activate the shader before setting uniforms!
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
