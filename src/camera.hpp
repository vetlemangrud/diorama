#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
struct Camera {
  glm::vec3 position;
  float pitch;
  float yaw;
};
void initCamera();
void moveUp (float amount);
void moveDown (float amount);
void moveForwards (float amount);
void moveBackwards (float amount);
void lookUp (float angle);
void lookDown (float angle);
void lookRight (float angle);
void lookLeft (float angle);
glm::mat4 getCameraViewMatrix();
