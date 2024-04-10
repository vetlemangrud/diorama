#include "glm/ext/vector_float4.hpp"
#include "utilities/shader.hpp"

struct ComputeOutput {
  alignas(16) glm::vec4 transformedPosition;
  alignas(16) glm::vec4 color;
};

void initScreen(Gloom::Shader *computeShader, Gloom::Shader *renderShader);

void renderScreen(Gloom::Shader *computeShader, Gloom::Shader *renderShader,
                  int numSplats);
