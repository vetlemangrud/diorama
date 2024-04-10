// The screen is the rectangle that covers the whole screen (We render the
// graphics in a fragment shader that runs on this screen)

#include "screen.hpp"
#include "shader.hpp"
#include <cmath>

unsigned int VBO;
unsigned int VAO;
void initScreen(Gloom::Shader *computeShader, Gloom::Shader *renderShader) {

  computeShader->attach("../assets/shaders/transform.comp");
  computeShader->link();

  renderShader->attach("../assets/shaders/simple.vert");
  renderShader->attach("../assets/shaders/simple.frag");
  renderShader->link();

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  const float vertices[] = {
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void renderScreen(Gloom::Shader *computeShader, Gloom::Shader *renderShader,
                  int numSplats) {

  computeShader->activate();
  int numWorkGroupsX =
      ceil(float(numSplats) / 64); // Assuming local_size_x = 64
  // Dispatch work groups
  glDispatchCompute(numWorkGroupsX, 1, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

  renderShader->activate();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
}
