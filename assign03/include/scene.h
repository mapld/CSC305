#pragma once
#include <vector>
#include "models_2D.h"


struct Scene{
  std::vector<Model> models;
  Model pointModel;
  std::vector<glm::mat4> mvps;
  std::vector<glm::vec3> points;
  std::vector<glm::vec3> linePoints;
  GLuint lineVBO = 0;
  GLuint lineIBO = 0;
  glm::mat4 lineMVP;

  bool actionDown = false;
  int pointUnderMouse = -1;
};
