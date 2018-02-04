#pragma once
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include "gr_lib.h"

using namespace OpenGP;
struct Triangle {
  Vec3 v1, v2, v3, n1, n2, n3;
};

float triangleArea(Vec3 v1, Vec3 v2, Vec3 v3) {
  return 0.5f*(v1(0)*v2(1) + v2(0)*v3(1) + v3(0)*v1(1) - (v1(0)*v3(1) + v2(0)*v1(1) + v3(0)*v2(1)));
}
