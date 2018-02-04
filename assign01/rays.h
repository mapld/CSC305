#pragma once
#include "ray.h"
#include "world.h"
#include "OpenGP/Image/Image.h"

struct WorldCoordsCamera{
  OpenGP::Vec3 origin;
  OpenGP::Vec3 direction;
  OpenGP::Vec3 up;
  OpenGP::Vec3 right;

  float aspectRatio;

  int imageCols;
  int imageRows;
};

WorldCoordsCamera createWorldCoordsCamera(World& world, OpenGP::Image<Colour>& image){
  Camera camera = world.camera;
  WorldCoordsCamera wcc;
  wcc.origin = camera.position+camera.eye;
  wcc.direction = (camera.position + camera.focus) - wcc.origin;
  wcc.direction.normalize();
  wcc.up = camera.up;
  wcc.right = camera.up.cross(wcc.direction); // right
  wcc.aspectRatio = (float)world.wResolution / (float)world.hResolution;
  wcc.imageCols = image.cols();
  wcc.imageRows = image.rows();
  return wcc;
}

Ray getRayForPixel(int row, int col, WorldCoordsCamera wcc){
  Ray ray;

  OpenGP::Vec3 pixelImageSpace = OpenGP::Vec3((float)col/wcc.imageCols-0.5f,(float)row/wcc.imageRows-0.5f, 0);
  OpenGP::Vec3 pixelPos = pixelImageSpace(0) * wcc.right * wcc.aspectRatio
    + pixelImageSpace(1) * wcc.up
    + wcc.origin
    + wcc.direction;

  ray.direction = pixelPos - wcc.origin; // direction of ray
  ray.direction.normalize();
  ray.origin = wcc.origin;
  return ray;
}
