#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>

#include "Game.h"
#include "ResourceManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);

const unsigned int SCREEN_WIDTH = 600;
const unsigned int SCREEN_HEIGHT = 600;

Game Among(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char* argv[]) {
  srand(time(0));

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, false);

  GLFWwindow* window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Among", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Among.Init();

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    Among.ProcessInput(deltaTime);

    Among.Update(deltaTime);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Among.Render();

    glfwSwapBuffers(window);
  }

  ResourceManager::Clear();

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      Among.Keys[key] = true;
    else if (action == GLFW_RELEASE)
      Among.Keys[key] = false;
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
