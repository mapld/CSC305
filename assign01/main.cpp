#include "OpenGP/Image/Image.h"
#include "bmpwrite.h"

using namespace OpenGP;

using Colour = Vec3; // RGB Value
Colour red() { return Colour(1.0f, 0.0f, 0.0f); }
Colour white() { return Colour(1.0f, 1.0f, 1.0f); }
Colour black() { return Colour(0.0f, 0.0f, 0.0f); }

int main(int, char**){
    int wResolution = 640;
    int hResolution = 480;
    float aspectRatio = (float)wResolution / (float)hResolution;
    // #rows = hResolution, #cols = wResolution
    Image<Colour> image(hResolution, wResolution);

    Vec3 spherePos = Vec3(0, 0, 4);
    float radius = 1.0f;

    Vec3 W = Vec3(0,0,-1); // opposite view direction
    Vec3 V = Vec3(0,1,0); // coplanar with w and up
    Vec3 U = Vec3(1,0,0);
    float d = 1.0f;

    Vec3 cameraPos = d * W; //Vec3(0, 0, 0);

    Vec3 llc = Vec3(-U(0)*aspectRatio, -V(1), 0.0f); // lower left corner
    Vec3 urc = Vec3(U(0)*aspectRatio, V(1), 0.0f); // upper right corner

    Vec3 lightPos = Vec3(-1,1,2);
    float kd = 0.5f;
    float ka = 0.01f;

    for (int row = 0; row < image.rows(); ++row) {
        for (int col = 0; col < image.cols(); ++col) {

          Vec3 pixelImageSpace = Vec3((float)col/image.cols(),(float)row/image.rows(), 0);
          Vec3 pixelPos = Vec3(llc(0) + pixelImageSpace(0) * (urc(0)-llc(0)),
                               llc(1) + pixelImageSpace(1) * (urc(1)-llc(1)),
                               0);


          Vec3 o = cameraPos; // origin of ray
          Vec3 l = pixelPos - cameraPos; // direction of ray
          l.normalize();
          Vec3 c = spherePos; // center of sphere
          Scalar r = radius;

          Scalar hit1 = ((l.dot(o-c)));
          Scalar hit2 = ((o-c).norm());
          Scalar hit3 = (r);
          Scalar descriminant = hit1*hit1 - hit2*hit2 + hit3*hit3;

          if(descriminant < 0){
            image(row,col) = white();
          } else {
            Scalar d = -(l.dot(o-c)) - sqrt(descriminant);
            Vec3 hit = o+(d*l);
            Vec3 normal = (hit-c)/radius;
            Vec3 lightDir = (lightPos - hit);
            lightDir.normalize();
            float toScale = fmax(0,normal.dot(lightDir))*kd + ka;
            Color diffusedColor = red()*toScale;
            image(row,col) = diffusedColor;
          }
       }
    }

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
