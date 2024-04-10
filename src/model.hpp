#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
struct Splat {
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 normal;
  float f_values[48]; // Coefficients for a Spherical Harmonics
                               // function defining color
  float opacity;
  alignas(16) glm::vec3 scale;
  alignas(16) glm::quat rotation;
};

struct Model {
  std::vector<Splat> splats;
};
