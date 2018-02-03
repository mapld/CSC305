//#include <math.h>
#include <float.h>
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include "bmpwrite.h"
#include <iostream>
#include <vector>

using namespace OpenGP;

using Colour = Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }

struct Camera{
  Vec3 position = Vec3(0,0,-5);
  Vec3 focus = Vec3(0,0,0);
  Vec3 eye = Vec3(0,0.0f,-1.0f);
  Vec3 up = Vec3(0,1,0);
};

struct World{
  int wResolution;
  int hResolution;

  float fieldOfView = 80.0f; // degrees
  Camera camera;
};

struct Sphere{
  Vec3 position;
  float radius;

  float diffuseFactor = 0.5f;
  float ambientFactor = 0.2f;
  float specularFactor = 0.2f;
  float phongExponent = 100.0f;
};

struct Plane{
  Vec3 point;
  Vec3 normal;

  float diffuseFactor = 0.5f;
  float ambientFactor = 0.2f;
  float specularFactor = 0.2f;
  float phongExponent = 100.0f;
};

Vec3 mat4MultVec3(Mat4x4 mat, Vec3 vec){
   Vec4 ssv = mat * Vec4(vec(0),vec(1),vec(2),1);
   Vec3 result = Vec3(ssv(0)/ssv(3),ssv(1)/ssv(3),ssv(2)/ssv(3));
   std::cout << result(0) << ", " << result(1) << ", " << result(2) << "\n";
   return result;
}

void printVec3(Vec3 vec){
  std::cout << vec(0) << ", " << vec(1) << ", " << vec(2) << "\n";
}

void drawPlane(World world, Image<Colour>& image, Image<float> &depth, Plane plane,std::vector<Vec3> lights){
  float aspectRatio = (float)world.wResolution / (float)world.hResolution;

  Camera camera = world.camera;

  Vec3 cameraOrigin = camera.position+camera.eye;
  Vec3 cameraDirection = (camera.position + camera.focus) - cameraOrigin;
  cameraDirection.normalize();
  Vec3 cameraU = camera.up;
  Vec3 cameraR = cameraU.cross(cameraDirection); // right

  for (int row = 0; row < image.rows(); ++row) {
    for (int col = 0; col < image.cols(); ++col) {
      Vec3 pixelImageSpace = Vec3((float)col/image.cols()-0.5f,(float)row/image.rows()-0.5f, 0);
      Vec3 pixelPos = pixelImageSpace(0) * cameraR * aspectRatio
        + pixelImageSpace(1) * cameraU
        + cameraOrigin
        + cameraDirection;

      Vec3 rayDirection = pixelPos - cameraOrigin; // direction of ray
      rayDirection.normalize();

      float denominator = plane.normal.dot(rayDirection);
      if(fabs(denominator) > 0.0001f){
        Vec3 p = (plane.point - cameraOrigin);
        float t = p.dot(plane.normal) / denominator;
        if(t >= 0){
          Vec3 hit = t * rayDirection;

          float z = t; //(hit(2)-cameraOrigin(2));// / rayDirection(2);
          if(z > depth(row,col)){
            std::cout << "z: " << z << "\n";
            std::cout << "depth: " << depth(row,col) << "\n";
            continue;
          }
          depth(row,col) = z;
          // std::cout << "t: " << t << "\n";
          // std::cout << "depth: " << z << "\n";

          float diffuseFactor = plane.diffuseFactor;
          float ambientFactor = plane.ambientFactor;
          float specularFactor = plane.specularFactor;
          float phongExponent = plane.phongExponent;

          Vec3 lightPos = lights[0];

          Vec3 lightDirection = (lightPos - hit);
          lightDirection.normalize();

          Vec3 v = cameraOrigin-hit;
          v.normalize();
          Vec3 half = lightDirection + v;
          half.normalize();

          float scaleFactor = fmax(0,plane.normal.dot(lightDirection))*diffuseFactor // diffuse lighting
            + fmax(0, pow(plane.normal.dot(half), phongExponent))*specularFactor // specular lighting
            + ambientFactor;
          Color diffusedColor = white()*scaleFactor;
          image(row,col) = diffusedColor;
        }
      }

    }
  }
}

void drawSpheres(World world, Image<Colour>& image, Image<float> &depth, std::vector<Vec3> lights, std::vector<Sphere> spheres){
  float aspectRatio = (float)world.wResolution / (float)world.hResolution;

  Camera camera = world.camera;

  Vec3 cameraOrigin = camera.position+camera.eye;
  Vec3 cameraDirection = (camera.position + camera.focus) - cameraOrigin;
  cameraDirection.normalize();

  Vec3 cameraU = camera.up;
  Vec3 cameraR = cameraU.cross(cameraDirection); // right

  for (int row = 0; row < image.rows(); ++row) {
        for (int col = 0; col < image.cols(); ++col) {
          Vec3 pixelImageSpace = Vec3((float)col/image.cols()-0.5f,(float)row/image.rows()-0.5f, 0);
          Vec3 pixelPos = pixelImageSpace(0) * cameraR * aspectRatio
            + pixelImageSpace(1) * cameraU
            + cameraOrigin
            + cameraDirection;

          Vec3 o = cameraOrigin; // origin of ray
          Vec3 l = pixelPos - o; // direction of ray
          l.normalize();

          for(int sph = 0; sph < spheres.size(); ++sph){
            Sphere sphere = spheres[sph];
            Vec3 spherePos = sphere.position;
            float radius = sphere.radius;

            Vec3 c = spherePos; // center of sphere
            Scalar r = radius;  // radius of sphere

            Scalar c1 = ((l.dot(o-c)));
            Scalar c2 = ((o-c).norm());
            Scalar c3 = (r);
            // part of the t calculation under the square root
            Scalar descriminant = c1*c1 - c2*c2 + c3*c3;

            // if the descriminant is above 0, the ray l hits the sphere
            if(descriminant < 0){
              //image(row,col) = white();
            } else {
              float diffuseFactor = sphere.diffuseFactor;
              float ambientFactor = sphere.ambientFactor;
              float specularFactor = sphere.specularFactor;
              float phongExponent = sphere.phongExponent;

              Scalar t = -(l.dot(o-c)) - sqrt(descriminant); // time of the first hit
              Vec3 hit = o+(t*l); // location of the first hit

              float z = t;//(hit(2)-cameraOrigin(2));// / l(2);
              if(z > depth(row,col)){
                continue;
              }
              std::cout << "row: " << row << "\n";
              std::cout << "col: " << col << "\n";
              std::cout << "z: " << z << "\n";
              std::cout << "depth: " << depth(row,col) << "\n";
              depth(row,col) = z;

              // Unit normal of surface (normalized vector):
              Vec3 normal = (hit-c)/radius;

              Vec3 lightPos = lights[0];

              Vec3 lightDirection = (lightPos - hit);
              lightDirection.normalize();

              Vec3 v = o-hit;
              v.normalize();
              Vec3 half = lightDirection + v;
              half.normalize();

              float scaleFactor = fmax(0,normal.dot(lightDirection))*diffuseFactor // diffuse lighting
                + fmax(0, pow(normal.dot(half), phongExponent))*specularFactor // specular lighting
                                  + ambientFactor;                                 // ambient lighting
              Color diffusedColor = red()*scaleFactor;
              image(row,col) = diffusedColor;
            }
          }
       }
    }
}

int main(int, char**){
    World world;
    world.wResolution = 640;
    world.hResolution = 480;

    Image<Colour> image(world.hResolution, world.wResolution);
    Image<float> depth(world.hResolution, world.wResolution);

    for (int i = 0;i < depth.cols();i++) {
      for (int j = 0;j < depth.rows();j++) {
        depth(j,i) = FLT_MAX;
      }
    }

    Sphere sphere;
    sphere.position = Vec3(0, -4, 5);
    sphere.radius = 1.0f;

    Sphere sphere2;
    sphere2.position = Vec3(0, 0, 5);
    sphere2.radius = 1.0f;

    std::vector<Sphere> spheres;
    spheres.push_back(sphere);
    spheres.push_back(sphere2);

    Plane plane;
    plane.point = Vec3(0,-4.5f,0);
    plane.normal = Vec3(0,1,0);
    // plane.point = Vec3(-100,0,0);
    // plane.normal = Vec3(1,0,0);

    // Single light source for now
    std::vector<Vec3> lights = {Vec3(0.0f,3.0f,2)};

    drawPlane(world, image, depth, plane, lights);
    drawSpheres(world, image, depth, lights, spheres);

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
