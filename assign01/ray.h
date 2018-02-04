#pragma once
#include "OpenGP/Image/Image.h"

struct Ray{
  OpenGP::Vec3 origin;
  OpenGP::Vec3 direction;
};

Ray newRay(OpenGP::Vec3 origin, OpenGP::Vec3 direction){
  Ray ray;
  ray.origin = origin;
  ray.direction = direction;
  return ray;
}
