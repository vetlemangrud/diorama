#include "GLFW/glfw3.h"
#include "diorama.hpp"
#include "camera.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "utilities/modelLoader.hpp"
#include "utilities/shader.hpp"
#include "utilities/window.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

SplatData model;
std::vector<glm::vec4> transformed;

Gloom::Shader *renderShader;

unsigned int VAO; // Vertex array for the "screen"
unsigned int VBO; // Vertex buffer for the positions of the corners
float explosionProgress;
float explosionPower;
bool explosionHappening;
glm::vec3 gravity;


void initDiorama(GLFWwindow *window, CommandLineOptions options) {
  explosionHappening = false;
  gravity = glm::vec3(0,0.002,0);
  model = loadModel(
      "../assets/vetle.ply"); // Asset by Kevin Kwok (https://antimatter15.com/)
  initCamera();
    moveBackwards(6);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  float cube[] = {
      // Back
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,

      // Front
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,

      // Bottom
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,

      // Top
      -1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      -1.0f,

      // Left
      -1.0f,
      -1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      -1.0f,
      -1.0f,
      -1.0f,

      // Right
      1.0f,
      -1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      1.0f,
      -1.0f,
      1.0f,
      1.0f,
      -1.0f,
      -1.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

  renderShader = new Gloom::Shader();
  renderShader->attach("../assets/shaders/simple.vert");
  renderShader->attach("../assets/shaders/simple.frag");
  renderShader->link();
  renderShader->activate();

  GLuint positionSSBO;
  glGenBuffers(1, &positionSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.positions.size() * sizeof(glm::vec4),
               model.positions.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionSSBO);

  GLuint scaleSSBO;
  glGenBuffers(1, &scaleSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, scaleSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.scales.size() * sizeof(glm::vec4), model.scales.data(),
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scaleSSBO);
  std::cout << glm::to_string(model.scales.at(0)) << std::endl;

  GLuint fValuesSSBO;
  glGenBuffers(1, &fValuesSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, fValuesSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.f_values.size() * sizeof(F_Values), model.f_values.data(),
               GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, fValuesSSBO);

  GLuint rotationsSSBO;
  glGenBuffers(1, &rotationsSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, rotationsSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.rotations.size() * sizeof(glm::vec4),
               model.rotations.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, rotationsSSBO);

  GLuint opacitiesSSBO;
  glGenBuffers(1, &opacitiesSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, opacitiesSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, model.opacities.size() * sizeof(float),
               model.opacities.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, opacitiesSSBO);
}

void updateFrame(GLFWwindow *window) {
  if(explosionHappening){
    explosionProgress +=1;
  }
}

void renderFrame(GLFWwindow *window) {
  int modelMatrixLocation = renderShader->getUniformFromName("model");
  int viewMatrixLocation = renderShader->getUniformFromName("view");
  int projectionMatrixLocation = renderShader->getUniformFromName("projection");
  glm::mat4 projection =
      glm::perspective(glm::radians(80.f),
                       float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
  glm::mat4 viewMatrix = getCameraViewMatrix();
  glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0),glm::vec3(-1,-1,1));

  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
                     glm::value_ptr(modelMatrix));
  glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE,
                     glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE,
                     glm::value_ptr(projection));


  int isExplodingLocation = renderShader->getUniformFromName("isExploding");
  int explosionPowerLocation = renderShader->getUniformFromName("explosionPower");
  int explosionProgressLocation = renderShader->getUniformFromName("explosionProgress");
  int gravityLocation = renderShader->getUniformFromName("g");
  int centerLocation = renderShader->getUniformFromName("center");
  glUniform1i(isExplodingLocation, explosionHappening);
  glUniform1f(explosionPowerLocation, explosionPower);
  glUniform1f(explosionProgressLocation, explosionProgress);
  glUniform3fv(gravityLocation, 1, glm::value_ptr(gravity));
  glUniformMatrix4fv(centerLocation, 1, GL_FALSE,
                     glm::value_ptr(model.center));
  // Run render shader
  glBindVertexArray(VAO);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 36, model.positions.size());
}

void explode(float power) {
  explosionHappening = true;
  explosionProgress = 0;
  explosionPower = power;

}

void stopExploding(){
  explosionHappening = false;
}
