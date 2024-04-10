#include "GLFW/glfw3.h"
#include "fmt/core.h"
#include "glad/glad.h"
#include "glm/ext/matrix_projection.hpp"
#include "utilities/modelLoader.hpp"
#include "utilities/screen.hpp"
#include "utilities/shader.hpp"
#include "utilities/window.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Model model;
std::vector<glm::vec4> transformed;

Gloom::Shader *computeShader;
Gloom::Shader *renderShader;
void initDiorama(GLFWwindow *window, CommandLineOptions options) {
  computeShader = new Gloom::Shader();
  renderShader = new Gloom::Shader();
  initScreen(computeShader, renderShader); // Setup the geometry the screen is
                                           // rendered on and the shaders
  model = loadModel(
      "../assets/sled.ply"); // Asset by Kevin Kwok (https://antimatter15.com/)

  // Shader Storage Buffer Object to pass all the splats to the shaders
  // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
  GLuint ssbo;
  GLuint ssbo1;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, model.splats.size() * sizeof(Splat),
               model.splats.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  glGenBuffers(1, &ssbo1);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.splats.size() * sizeof(ComputeOutput), NULL,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo1);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void updateFrame(GLFWwindow *window) {}

void renderFrame(GLFWwindow *window) {
  int widthLocation = renderShader->getUniformFromName("width");
  int heightLocation = renderShader->getUniformFromName("height");
  glUniform1i(widthLocation, windowWidth);
  glUniform1i(heightLocation, windowHeight);
  int ViewMatrixLocation = computeShader->getUniformFromName("ViewMatrix");
  int ProjectionMatrixLocation = computeShader->getUniformFromName("ProjectionMatrix");
  glm::mat4 projection =
      glm::perspective(glm::radians(80.0f),
                       float(windowWidth) / float(windowHeight), 0.1f, 350.f);
  glm::mat4 cameraTransform = glm::mat4(1.0);
  cameraTransform = glm::translate(cameraTransform, glm::vec3(0, 0, -5));
  glm::mat4 VP = cameraTransform;

  glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(cameraTransform));
  glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
  renderScreen(computeShader, renderShader, model.splats.size());
}
