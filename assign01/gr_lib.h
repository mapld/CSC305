#pragma once
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"

using Colour = OpenGP::Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour otherRed() { return Colour(1.0f, 0.1f, 0.0f); }
Colour green() { return Colour(0.0f, 1.0f, 0.0f); }
Colour orange() { return Colour(1.0f, 0.5f, 0.0f);}
Colour blue() { return Colour(0.0f, 0.0f, 1.0f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }

void printVec3(OpenGP::Vec3 vec){
  std::cout << vec(0) << ", " << vec(1) << ", " << vec(2) << "\n";
}

OpenGP::Vec3 mat4MultVec3(OpenGP::Mat4x4 mat, OpenGP::Vec3 vec){
  OpenGP::Vec4 ssv = mat * OpenGP::Vec4(vec(0),vec(1),vec(2),1);
  OpenGP::Vec3 result = OpenGP::Vec3(ssv(0)/ssv(3),ssv(1)/ssv(3),ssv(2)/ssv(3));
  return result;
}
