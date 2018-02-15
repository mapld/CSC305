//#include <math.h>
#include <float.h>
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include "bmpwrite.h"
#include <iostream>
#include <vector>
#include "gr_lib.h"
#include "world.h"
#include "draw.h"

using namespace OpenGP;

int main(int, char**){
    std::string meshName = "bunny.obj";

    SurfaceMesh mesh;
    if (!mesh.read(meshName)) {
      std::cerr << "Failed to read input mesh" << std::endl;
      return EXIT_FAILURE;
    }
    mesh.update_vertex_normals();

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

    world.camera.position = Vec3(0,0,-5);
    world.camera.focus = Vec3(0,0,0);
    world.camera.eye = Vec3(0,0,-1.0f);
    world.camera.up = Vec3(0,1,0);

    world.camera.up.normalize();

    Sphere sphere;
    sphere.position = Vec3(-1.5f, -4, 6);
    sphere.radius = 1.0f;
    sphere.colour = orange();

    Sphere sphere2;
    sphere2.position = Vec3(0, 0, 5);
    sphere2.radius = 1.0f;
    sphere2.colour = red();

    world.spheres.push_back(sphere);
    world.spheres.push_back(sphere2);

    Plane plane;
    plane.point = Vec3(0,-3.5f,0);
    plane.normal = Vec3(0,1,0);
    // plane.point = Vec3(-100,0,0);
    // plane.normal = Vec3(1,0,0);

    Mat4x4 model = OpenGP::translate(-2.0f,-3.33,5.5f) * OpenGP::scale(4.0f);

    Plane rightWall;
    rightWall.point = Vec3(4,0,0);
    rightWall.normal = Vec3(-1,0,0.0f);
    rightWall.normal.normalize();
    rightWall.colour = otherRed();

    Plane leftWall;
    leftWall.point = Vec3(-4,0,0);
    leftWall.normal = Vec3(1,0,0.0f);
    leftWall.normal.normalize();
    leftWall.colour = green();

    Plane backWall;
    backWall.point = Vec3(0,0,20);
    backWall.normal = Vec3(0,0,-1);

    Plane ceiling;
    ceiling.point = Vec3(0,10,0);
    ceiling.normal = Vec3(0,-1,0);
    ceiling.colour = blue();

    world.planes.push_back(rightWall);
    world.planes.push_back(leftWall);
    world.planes.push_back(plane);
    world.planes.push_back(backWall);
    world.planes.push_back(ceiling);

    // Single light source for now
    // world.lights = {Vec3(0.1f,9.9f,1.5f)};
    world.lights = {Vec3(3.5f,1.0f,1.5f)};
    // world.lights = {Vec3(0.0f,9.7f,17.7f)};

    drawPlanes(world, image, depth);
    drawSpheres(world, image, depth);
    drawPolygonMesh(world, mesh, model, image, depth);

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
