#pragma once
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include <vector>
#include "gr_lib.h"
#include "ray.h"

struct Sphere{
  OpenGP::Vec3 position;
  float radius;

  float diffuseFactor = 0.5f;
  float ambientFactor = 0.2f;
  float specularFactor = 0.2f;
  float phongExponent = 100.0f;

  Colour colour = white();
};

float rayHitsSphere(Ray ray, Sphere sphere){
  OpenGP::Vec3 spherePos = sphere.position;
  float radius = sphere.radius;

  OpenGP::Vec3 c = spherePos; // center of sphere
  float r = radius;  // radius of sphere

  float c1 = ((ray.direction.dot(ray.origin-c)));
  float c2 = ((ray.origin-c).norm());
  float c3 = (r);
  // part of the t calculation under the square root
  float descriminant = c1*c1 - c2*c2 + c3*c3;
  if(descriminant < 0){
    return descriminant;
  } else {
    float t = -(ray.direction.dot(ray.origin-c)) - sqrt(descriminant); // time of the first hit
    return t;
  }
}
