#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/gtc/quaternion.hpp>
#include "glm/matrix.hpp"
#include "model.hpp"
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

void compute3DCovariance(float covariance[6], float scale[3], float rotation[4]) {
  glm::mat3 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scale[0], scale[1], scale[2]));
  //
  // Compute rotation matrix from quaternion
  glm::mat3 rotationMatrix = glm::mat3_cast( glm::quat(rotation[0],rotation[1],rotation[2],rotation[3]));

  // In the "3D Gaussian Splatting for Real-Time Radiance Field Rendering" Cov3D = R * S * S' * R' (' is transposed)
  glm::mat3 covarianceMatrix = rotationMatrix * scaleMatrix * glm::transpose(scaleMatrix) * glm::transpose(rotationMatrix);

  //Get the top right of the matix, it is symmetrical
  covariance[0] = covarianceMatrix[0][0];
  covariance[1] = covarianceMatrix[1][0];
  covariance[2] = covarianceMatrix[2][0];
  covariance[3] = covarianceMatrix[1][1];
  covariance[4] = covarianceMatrix[2][1];
  covariance[5] = covarianceMatrix[2][2];
}    

SplatData loadModel(const std::string filename) {
  std::ifstream inputFile(filename, std::ios::binary);
  if (!inputFile.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    std::cerr << errno << std::endl;
  }

  // Assume that the given file is in the format outputted by Lumalabs

  // Go through header lines. Since we assume the format, we don't need the data
  // (exept vertex count)
  std::string line;
  int vertex_count;
  std::string COUNT_PROP = "element vertex";
  while (std::getline(inputFile, line)) {
    if (line == "end_header") {
      break;
    }
    if (line.substr(0, COUNT_PROP.length()) == COUNT_PROP) {

      vertex_count = std::stoi(line.substr(COUNT_PROP.length(), line.length()));
    }
  }
  
  SplatData data;
  data.positions = {};
  data.normals = {};
  data.f_values = {}; 

  // Read splats
  const int PROPERTY_COUNT = 62;
  for (int i = 0; i < vertex_count; i++) {
    float values[PROPERTY_COUNT];

    inputFile.read(reinterpret_cast<char *>(&values),
                   sizeof(float) * PROPERTY_COUNT);
    data.positions.push_back( glm::vec4(values[0], values[1], values[2],1.0));
    data.normals.push_back(glm::vec4(values[3], values[4], values[5],1.0));
    F_Values f_values;
    for (int i = 6; i < 3 + 6; i++) {
      f_values.values[i - 6] = values[i];
    }
    data.f_values.push_back(f_values);
    data.opacities.push_back(values[54]);
    data.scales.push_back(glm::exp(glm::vec4(values[55], values[56], values[57],1.0)));
    data.rotations.push_back(glm::normalize(glm::vec4(values[58], values[59], values[60], values[61])));

    float covariance[6];
    //compute3DCovariance(covariance, scale, rotation);
    //data.covariances.push_back(covariance);
  }


  inputFile.close();
  return data;
}
