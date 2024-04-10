#include "model.hpp"
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>

Model loadModel(const std::string filename) {
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
    // std::cout << "Header line: " << line << std::endl;
  }
  std::vector<Splat> splats = {};

  // Read splats
  int PROPERTY_COUNT = 62;
  for (int i = 0; i < vertex_count; i++) {
    float values[PROPERTY_COUNT];

    inputFile.read(reinterpret_cast<char *>(&values),
                   sizeof(float) * PROPERTY_COUNT);
    Splat splat;
    splat.position = glm::vec3(values[0], values[1], values[2]);
    splat.normal = glm::vec3(values[3], values[4], values[5]);
    for (int i = 6; i < 48 + 6; i++) {
      splat.f_values[i - 6] = values[i];
    }
    splat.opacity = values[54];
    splat.scale = glm::vec3(values[55], values[56], values[57]);
    splat.rotation = glm::quat(values[58], values[59], values[60], values[61]);

    splats.push_back(splat);
  }

  float max_opa = -9999;
  float min_opa = 9999;
  for (int i = 0; i < vertex_count; i++) {
    if (splats.at(i).opacity > max_opa) {
      max_opa = splats.at(i).opacity;
    }
    if (splats.at(i).opacity < min_opa) {
      min_opa = splats.at(i).opacity;
    }
  }

  inputFile.close();
  Model model;
  model.splats = splats;
  return model;
}
