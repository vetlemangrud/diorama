#include "glm/ext/vector_float4.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

struct F_Values {
  float values[48];
};

struct SplatData {
  std::vector<glm::vec4> positions;
  std::vector<glm::vec4> normals;
  std::vector<F_Values> f_values;
  std::vector<float> opacities;
  std::vector<glm::vec4> scales;
  std::vector<glm::vec4> rotations;
  glm::vec4 center;
};
