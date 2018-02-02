#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include "bmpwrite.h"
#include <iostream>

using namespace OpenGP;

using Colour = Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }

Vec3 mat4MultVec3(Mat4x4 mat, Vec3 vec){
   Vec4 ssv = mat * Vec4(vec(0),vec(1),vec(2),1);
   Vec3 result = Vec3(ssv(0)/ssv(3),ssv(1)/ssv(3),ssv(2)/ssv(3));
   std::cout << result(0) << ", " << result(1) << ", " << result(2) << "\n";
   return result;
}

void printVec3(Vec3 vec){
  std::cout << vec(0) << ", " << vec(1) << ", " << vec(2) << "\n";
}

int main(int, char**){
    // screen dimensions:
    int wResolution = 640;
    int hResolution = 480;

    float aspectRatio = (float)wResolution / (float)hResolution;

    Image<Colour> image(hResolution, wResolution);

    Vec3 spherePos = Vec3(0, 0, 5);
    float radius = 1.0f;

    float fieldOfView = 80.0f; // degrees
    // float near = 0.1f;
    // float far = 5.0f;
    // Mat4x4 projection = OpenGP::perspective(fieldOfView, (float)wResolution/(float)hResolution, near, far);

    Vec3 cameraPosition = Vec3(0,5,-5);
    Vec3 focus = Vec3(0,0,0);
    Vec3 eye = Vec3(0,0.5f,-1.0f);

    // Single light source for now
    Vec3 lightPos = Vec3(-1,2.5f,2);

    Vec3 cameraOrigin = cameraPosition+eye;
    Vec3 cameraDirection = (cameraPosition + focus) - cameraOrigin;
    cameraDirection.normalize();
    // Vec3 cameraDirection = Vec3(0,0,1);

    Vec3 cameraU = Vec3(0,1,0); // up
    Vec3 cameraR = cameraU.cross(cameraDirection); // right
    //printVec3(cameraR);
    //cameraU = cameraR.cross(cameraDirection);

    // Mat4x4 model = Mat4x4::Identity();
    // Mat4x4 mvp = projection * view * model;

    // Mat4x4 view = OpenGP::lookAt(eye, focus, cameraU);
    // spherePos = mat4MultVec3(view, spherePos);
    // lightPos = mat4MultVec3(view, lightPos);

    // Phong lighting
    float diffuseFactor = 0.5f;
    float ambientFactor = 0.2f;
    float specularFactor = 0.4f;

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
            Scalar t = -(l.dot(o-c)) - sqrt(descriminant); // time of the first hit
            Vec3 hit = o+(t*l); // location of the first hit

            // Unit normal of surface (normalized vector):
            Vec3 normal = (hit-c)/radius;

            Vec3 lightDirection = (lightPos - hit);
            lightDirection.normalize();

            float scaleFactor = fmax(0,normal.dot(lightDirection))*diffuseFactor // diffuse lighting
                                + ambientFactor;                                 // ambient lighting
            Color diffusedColor = red()*scaleFactor;
            image(row,col) = diffusedColor;
          }
       }
    }

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
