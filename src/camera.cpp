#include "camera.hpp"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/scalar_constants.hpp"

Camera camera;
void initCamera() {
  camera.position = glm::vec3(0);
  camera.yaw = 0;
  camera.pitch = 0;
}
void moveUp(float amount) { camera.position.y += amount; }
void moveDown(float amount) { moveUp(-amount); }
void moveForwards(float amount) {
  glm::vec3 movement = glm::vec3(0, 0, -amount);
  glm::mat4 rotation = glm::rotate(glm::mat4(1.0), camera.yaw, glm::vec3(0, 1, 0));
  camera.position += glm::vec3(rotation * glm::vec4(movement,1.0));
}
void moveBackwards(float amount) { moveForwards(-amount); }
void lookUp(float angle) {
  camera.pitch += angle;
  camera.pitch = glm::max(camera.pitch, -glm::pi<float>()/2);
  camera.pitch = glm::min(camera.pitch, glm::pi<float>()/2);
}
void lookDown(float angle) { lookUp(-angle); }
void lookRight(float angle) { camera.yaw -= angle; }
void lookLeft(float angle) { lookRight(-angle); }

glm::mat4 getCameraViewMatrix() {

  // Create view matrix
  // Rotate
  glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), camera.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
  rotation = glm::rotate(rotation, camera.yaw, glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 view = glm::inverse(rotation);

  //Translate
  view = glm::translate(view, -camera.position);

  return view;
}
