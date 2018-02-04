#pragma once
#include <float.h>
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include <vector>
#include "gr_lib.h"
#include "plane.h"
#include "sphere.h"
#include "ray.h"

struct Camera{
  OpenGP::Vec3 position;
  OpenGP::Vec3 focus;
  OpenGP::Vec3 eye;
  OpenGP::Vec3 up;
  float fieldOfView = 90.0f; // degrees
};

struct World{
  int wResolution;
  int hResolution;

  Camera camera;

  std::vector<Sphere> spheres;
  std::vector<Plane> planes;
  std::vector<OpenGP::Vec3> lights;
};

bool underShadow(World& world, Ray ray, float maxT = FLT_MAX){
  for(int i = 0; i < world.spheres.size(); i++){
    Sphere sphere = world.spheres[i];
    float t = rayHitsSphere(ray, sphere);
    float thresh = 0.01f;
    if(t > thresh && t < maxT){
      return true;
    }
  }
  return false;
}
