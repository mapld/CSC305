#pragma once
#include "world.h"
#include "rays.h"
#include "triangles.h"
#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include <iostream>

using namespace OpenGP;

void drawPlanes(World& world, Image<Colour>& image, Image<float> &depth){
  WorldCoordsCamera wcc = createWorldCoordsCamera(world, image);

  for (int row = 0; row < image.rows(); ++row) {
    for (int col = 0; col < image.cols(); ++col) {
      Ray ray = getRayForPixel(row, col, wcc);

      for(int pln = 0; pln < world.planes.size(); ++pln){
        Plane plane = world.planes[pln];
        float denominator = plane.normal.dot(ray.direction);
        if(fabs(denominator) > 0.0001f){
          Vec3 p = (plane.point - wcc.origin);
          float t = p.dot(plane.normal) / denominator;
          if(t >= 0){
            Vec3 hit = wcc.origin + (t * ray.direction);

            float z = t;
            if(z > depth(row,col)){
              continue;
            }
            depth(row,col) = z;

            float diffuseFactor = plane.diffuseFactor;
            float ambientFactor = plane.ambientFactor;
            float specularFactor = plane.specularFactor;
            float phongExponent = plane.phongExponent;

            Vec3 lightPos = world.lights[0];

            Vec3 lightDirection = (lightPos - hit);
            float maxT = lightDirection.norm();
            lightDirection.normalize();

            Vec3 v = wcc.origin-hit;
            v.normalize();
            Vec3 half = lightDirection + v;
            half.normalize();


            float scaleFactor = ambientFactor;
            if(!underShadow(world, newRay(hit, lightDirection), maxT)){
              scaleFactor += fmax(0,plane.normal.dot(lightDirection))*diffuseFactor // diffuse lighting
                + fmax(0, pow(plane.normal.dot(half), phongExponent))*specularFactor; // specular lighting
            }
            Color diffusedColor = plane.colour*scaleFactor;
            image(row,col) = diffusedColor;
          }
        }
      }

    }
  }
}

void drawSpheres(World world, Image<Colour>& image, Image<float> &depth){
    WorldCoordsCamera wcc = createWorldCoordsCamera(world, image);

    for (int row = 0; row < image.rows(); ++row) {
        for (int col = 0; col < image.cols(); ++col) {
          Ray ray = getRayForPixel(row, col, wcc);

          Vec3 o = ray.origin; // origin of ray
          Vec3 l = ray.direction; // direction of ray
          l.normalize();

          for(int sph = 0; sph < world.spheres.size(); ++sph){
            Sphere sphere = world.spheres[sph];
                        // if the descriminant is above 0, the ray l hits the sphere
            float t = rayHitsSphere(ray,sphere);
            if(t > 0){
              float diffuseFactor = sphere.diffuseFactor;
              float ambientFactor = sphere.ambientFactor;
              float specularFactor = sphere.specularFactor;
              float phongExponent = sphere.phongExponent;

              Vec3 hit = o+(t*l); // location of the first hit

              float z = t;
              if(z > depth(row,col)){
                continue;
              }
              // std::cout << "z: " << z << "\n";

              depth(row,col) = z;

              // Unit normal of surface (normalized vector):
              Vec3 normal = (hit-sphere.position)/sphere.radius;

              Vec3 lightPos = world.lights[0];

              Vec3 lightDirection = (lightPos - hit);
              lightDirection.normalize();

              Vec3 v = o-hit;
              v.normalize();
              Vec3 half = lightDirection + v;
              half.normalize();

              float scaleFactor = ambientFactor;
              if(!underShadow(world, newRay(hit, lightDirection))){
                scaleFactor += fmax(0,normal.dot(lightDirection))*diffuseFactor // diffuse lighting
                + fmax(0, pow(normal.dot(half), phongExponent))*specularFactor; // specular lighting
              }
              Color diffusedColor = sphere.colour*scaleFactor;
              image(row,col) = diffusedColor;
            }
          }
       }
    }
}

void rasterizeTriangle(Triangle t, Image<Colour> &image, Image<float> &depth, Mat4x4 to_world, World& world) {
    Vec3 s1 = Vec3(t.v1(0), t.v1(1), 0);
    Vec3 s2 = Vec3(t.v2(0), t.v2(1), 0);
    Vec3 s3 = Vec3(t.v3(0), t.v3(1), 0);

    int maxCols = (int)image.cols();
    int maxRows = (int)image.rows();
    int iMin = (std::min(s1(0),std::min(s2(0),s3(0))) +1.0f)*0.5f*image.cols();
    iMin = std::max(iMin, 0);
    int iMax = (std::max(s1(0),std::max(s2(0),s3(0))) + 1.0f)*0.5f*image.cols();
    iMax = std::min(iMax+1, maxCols);
    int jMin = (std::min(s1(1),std::min(s2(1),s3(1))) + 1.0f)*0.5f*image.rows();
    jMin = std::max(jMin, 0);
    int jMax = (std::max(s1(1),std::max(s2(1),s3(1))) + 1.0f)*0.5f*image.rows();
    jMax = std::min(jMax+1, maxRows);

    float totalArea = triangleArea(s1, s2, s3);

    for (int i = iMin;i < iMax;i++) {
        for (int j = jMin;j < jMax;j++) {

            // Location of fragment in image space
            Vec3 pt = Vec3(((float)i) / image.cols(), ((float)j) / image.rows(), 0.0);
            pt = 2 * (pt - Vec3(0.5,0.5,0));

            // Barycentric coordinates of fragment
            float alpha = triangleArea(pt,s2,s3) / totalArea;
            float beta = triangleArea(s1,pt,s3) / totalArea;
            float gamma = triangleArea(s1,s2,pt) / totalArea;


            if (alpha>=0.0f && alpha<=1.0f && beta>=0.0f && beta<=1.0f && gamma>=0.0f &&gamma<=1.0f){

                Vec3 hit = Vec3(alpha*t.v1(0) + beta*t.v2(0) + gamma*t.v3(0),
                                alpha*t.v1(1) + beta*t.v2(1) + gamma*t.v3(1),
                                alpha*t.v1(2) + beta*t.v2(2) + gamma*t.v3(2));
                Vec3 hit_world = mat4MultVec3(to_world, hit);

                Vec3 cameraOrigin = (world.camera.eye + world.camera.position);
                Vec3 ray_world = hit_world - cameraOrigin;

                float z = ray_world.norm();

                if(z > depth(j,i)){
                  continue;
                }

                // std::cout << "z: " << z << "\n";
                // printVec3(hit_world);

                // Flat shading normal
                //Vec3 normal = t.n1 + t.n2 + t.n3;
                Vec3 normal = alpha*t.n1 + beta*t.n2 + gamma*t.n3;
                normal = normal.normalized();

                Vec3 lightPos = world.lights[0];

                Vec3 lightDirection = (lightPos - hit_world);
                // std::cout << "light direction: ";
                // printVec3(lightDirection);
                lightDirection.normalize();

                float ambientFactor = 0.2f;
                float diffuseFactor = 0.5f;
                float specularFactor = 0.2f;
                float phongExponent = 100.0f;

                Vec3 v = cameraOrigin-hit_world;
                v.normalize();
                Vec3 half = lightDirection + v;
                half.normalize();

                float scaleFactor = ambientFactor;
                if(!underShadow(world, newRay(hit_world, lightDirection))){
                  scaleFactor += fmax(0,normal.dot(lightDirection))*diffuseFactor // diffuse lighting
                    + fmax(0, pow(normal.dot(half), phongExponent))*specularFactor; // specular lighting
                }
                Vec3 c = Vec3(1.0f,1.0f,1.0) * scaleFactor;

                /// Visualize normals / regular shading option
                // image(j, i) = 0.5f*(Colour(normal(0),normal(1),normal(2))+Colour(1.0f, 1.0f, 1.0f));
                // image(j, i) = shadeFragment(normal);
                image(j, i) = c;

                // Set depth of fragment
                depth(j, i) = z;
            }
        }
    }
}

void drawPolygonMesh(World& world, SurfaceMesh& mesh, Mat4x4 model, Image<Colour>& image, Image<float>& depth){
  auto vpoint = mesh.vertex_property<Vec3>("v:point");
  auto vnormal = mesh.vertex_property<Vec3>("v:normal");

  float near = 0.1f;
  float far = 5.0f;
  Mat4x4 projection = OpenGP::perspective(world.camera.fieldOfView, (float)world.wResolution/(float)world.hResolution, near, far);

  Mat4x4 view = OpenGP::lookAt(world.camera.eye, world.camera.focus, world.camera.up);


  Mat4x4 mvp = projection * view * model;
  // Mat4x4 mvp_inverse = mvp.inverse();
  Mat4x4 to_world = (projection * view).inverse();

  for (SurfaceMesh::Face tri : mesh.faces()) {
        auto h1 = mesh.halfedge(tri);
        auto h2 = mesh.next_halfedge(h1);
        auto h3 = mesh.next_halfedge(h2);

        SurfaceMesh::Halfedge halfedges[3] = {
            h1, h2, h3
        };

        Vec3 vertices[3] = {
            Vec3(0,0,0), Vec3(0,0,0), Vec3(0,0,0)
        };

        Vec3 normals[3] = {
            Vec3(0,0,0), Vec3(0,0,0), Vec3(0,0,0)
        };

        /// Why don't we apply the mvp transformation to the normals?
        /// What transformations should apply to the normals?
        for (int i = 0;i < 3;i++) {
            auto vi = mesh.to_vertex(halfedges[i]);
            Vec3 v = vpoint[vi];
            Vec4 ssv = mvp * Vec4(v(0), v(1), v(2), 1);
            vertices[i] = Vec3(ssv(0) / ssv(3), ssv(1) / ssv(3), ssv(2) / ssv(3));
            normals[i] = vnormal[vi];
        }

        Triangle t = {
            vertices[0], vertices[1], vertices[2],
            normals[0], normals[1], normals[2]
        };

        rasterizeTriangle(t, image, depth, to_world, world);
    }
}
