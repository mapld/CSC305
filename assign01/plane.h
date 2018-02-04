#pragma once
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include <vector>
#include "gr_lib.h"

struct Plane{
  OpenGP::Vec3 point;
  OpenGP::Vec3 normal;

  float diffuseFactor = 0.5f;
  float ambientFactor = 0.4f;
  float specularFactor = 0.2f;
  float phongExponent = 100.0f;

  Colour colour = white();
};
