#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
struct Splat {
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 normal;
  float f_values[48]; // Coefficients for a Spherical Harmonics
                               // function defining color
  float opacity;
  // Only store covariance directly because we don't need scale and rotation for rasterization
  // alignas(16) glm::vec3 scale;
  // alignas(16) glm::quat rotation;
  float covariance[6];
};

struct ComputeOutput {
  alignas(16) glm::vec4 transformedPosition;
  alignas(16) glm::vec3 cov;
  alignas(16) glm::vec3 conic;
  alignas(16) glm::vec4 color;
};

struct Model {
  std::vector<Splat> splats;
};
