#include "program.hpp"
#include "GLFW/glfw3.h"
#include "camera.hpp"
#include "diorama.hpp"
#include "utilities/modelLoader.hpp"
#include "utilities/window.hpp"
#include <SFML/Audio.hpp>
#include <SFML/System/Time.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utilities/glutils.h>
#include <utilities/shader.hpp>
#include <utilities/shapes.h>
#include <utilities/timeutils.h>

void runProgram(GLFWwindow *window, CommandLineOptions options) {
  // Enable depth (Z) buffer (accept "closest" fragment)
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Configure miscellaneous OpenGL settings
  glEnable(GL_CULL_FACE);

  // Disable built-in dithering
  glDisable(GL_DITHER);

  // Enable transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set default colour after clearing the colour buffer
  glClearColor(0.3f, 0.5f, 0.8f, 1.0f);

  initDiorama(window, options);

  // Rendering Loop
  while (!glfwWindowShouldClose(window)) {
    // Clear colour and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateFrame(window);
    renderFrame(window);

    // Handle other events
    glfwPollEvents();
    handleKeyboardInput(window);

    // Flip buffers
    glfwSwapBuffers(window);
  }
}

void handleKeyboardInput(GLFWwindow *window) {
  // Use escape key for terminating the GLFW window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    moveForwards(0.03);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    moveBackwards(0.03);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    lookLeft(0.01);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    lookRight(0.01);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    moveUp(0.02);
  }
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
    moveDown(0.02);
  }
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
    lookDown(0.01);
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    lookUp(0.01);
  }
  if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
    explode(0.000000001);
  }
  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
    stopExploding();
  }
}
