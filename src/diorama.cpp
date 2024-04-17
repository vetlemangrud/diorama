#include "GLFW/glfw3.h"
#include "fmt/core.h"
#include "glad/glad.h"
#include "glm/ext/matrix_projection.hpp"
#include "utilities/modelLoader.hpp"
#include "utilities/shader.hpp"
#include "utilities/window.hpp"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Model model;
std::vector<glm::vec4> transformed;

Gloom::Shader *computeShader;
Gloom::Shader *renderShader;

float camera_fovy;        // Camera field of view in the y-direction
GLuint splatSSBO;         // Shader Storage Buffer Object to store splats
GLuint ComputedSplatSSBO; // SSBO to store results of the compute shader (e.g.
                          // screen pos, color)
unsigned int VAO;         // Vertex array for the "screen"
unsigned int VBO;         // Vertex buffer for the positions of the corners

float hello;

void initDiorama(GLFWwindow *window, CommandLineOptions options) {
  hello = 0;
  model = loadModel(
      "../assets/sled.ply"); // Asset by Kevin Kwok (https://antimatter15.com/)

  camera_fovy = glm::radians(80.f);

  //Activate shaders
  computeShader = new Gloom::Shader();
  renderShader = new Gloom::Shader();
  
  //Compute shader for computing properties of each splat
  computeShader->attach("../assets/shaders/transform.comp");
  computeShader->link();

  //Render shader rendering fragments on the "screen"
  renderShader->attach("../assets/shaders/simple.vert");
  renderShader->attach("../assets/shaders/simple.frag");
  renderShader->link();

  //Setting up the VAO and VBO for the "screen"
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  const float vertices[] = {
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Shader Storage Buffer Object to pass all the splats to the shaders
  glGenBuffers(1, &splatSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, splatSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, model.splats.size() * sizeof(Splat),
               model.splats.data(), GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, splatSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // Shader Storage Buffer Object to store the calculated properties from the compute shader
  glGenBuffers(1, &ComputedSplatSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ComputedSplatSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               model.splats.size() * sizeof(ComputeOutput), NULL,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ComputedSplatSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void updateFrame(GLFWwindow *window) {hello += 0.1;}

void renderFrame(GLFWwindow *window) {

  // Send uniform variables to compute shader
  computeShader->activate();
  // Parameters for projecting the covariance to 2D
  float tan_fovy = tan(camera_fovy * 0.5);
  float tan_fovx = tan_fovy * float(windowWidth) / float(windowHeight);
  float focal_y = float(windowHeight) / (2 * tan_fovy);
  float focal_x = float(windowWidth / (2 * tan_fovx));
  int tan_fovyLocation = computeShader->getUniformFromName("tan_fovy");
  int tan_fovxLocation = computeShader->getUniformFromName("tan_fovx");
  int focal_yLocation = computeShader->getUniformFromName("focal_y");
  int focal_xLocation = computeShader->getUniformFromName("focal_x");
  glUniform1f(tan_fovyLocation, tan_fovy);
  glUniform1f(tan_fovxLocation, tan_fovx);
  glUniform1f(focal_yLocation, focal_y);
  glUniform1f(focal_xLocation, focal_x);

  // Projection matrixes
  int ViewMatrixLocation = computeShader->getUniformFromName("ViewMatrix");
  int ProjectionMatrixLocation =
      computeShader->getUniformFromName("ProjectionMatrix");
  glm::mat4 projection = glm::perspective(
      camera_fovy, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
  glm::mat4 viewMatrix = glm::mat4(1.0);
  viewMatrix = glm::translate(viewMatrix, glm::vec3(0., -2+hello, 0));
  glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE,
                     glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(ProjectionMatrixLocation, 1, GL_FALSE,
                     glm::value_ptr(projection));

  // Send uniform variables to render shader
  renderShader->activate();
  int widthLocation = renderShader->getUniformFromName("width");
  int heightLocation = renderShader->getUniformFromName("height");
  glUniform1i(widthLocation, windowWidth);
  glUniform1i(heightLocation, windowHeight);

  // Run compute shader
  computeShader->activate();
  int numWorkGroupsX =
      ceil(float(model.splats.size()) / 64); // Assuming local_size_x = 64
  // Dispatch work groups
  glDispatchCompute(numWorkGroupsX, 1, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

  //Run render shader
  renderShader->activate();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  // For debugging (Reading SSBO data):
  // std::vector<ComputeOutput> storage(1); // n is the size
  // glGetNamedBufferSubData(ComputedSplatSSBO, 0, 1 * sizeof(ComputeOutput),
  //                         storage.data());
  // std::cout << glm::to_string(model.splats.at(0).covariance);
  // std::cout << glm::to_string(storage.at(0).cov) << std::endl;

}
