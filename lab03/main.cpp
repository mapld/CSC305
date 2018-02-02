#include "OpenGP/Image/Image.h"
#include "OpenGP/GL/Eigen.h"
#include "bmpwrite.h"
#include <iostream>

using namespace OpenGP;
using Colour = Vec3;

struct Triangle {
    Vec3 v1, v2, v3, n1, n2, n3;
};

float triangleArea(Vec3 v1, Vec3 v2, Vec3 v3) {
    return 0.5f*(v1(0)*v2(1) + v2(0)*v3(1) + v3(0)*v1(1) - (v1(0)*v3(1) + v2(0)*v1(1) + v3(0)*v2(1)));
}

Colour shadeFragment(Vec3 normal) {
    Vec3 dir(0.5f, 0.5f, 1.0f);
    float l = std::min(std::max(0.8f*normal.dot(dir.normalized()) + 0.1f, 0.0f), 1.0f);
    Vec3 c = Vec3(1.0f,1.0f,1.0) * l;
    return c.cast<float>();
}

void rasterize(Triangle t, Image<Colour> &image, Image<float> &depth) {

    // Vectors projected onto z=0 plane
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

            float z = alpha*t.v1(2) + beta*t.v2(2) + gamma*t.v3(2);

            if (alpha>=0.0f && alpha<=1.0f && beta>=0.0f && beta<=1.0f && gamma>=0.0f &&gamma<=1.0f
                    && depth(j,i) > z) {

                // Flat shading normal
                //Vec3 normal = t.n1 + t.n2 + t.n3;
                Vec3 normal = alpha*t.n1 + beta*t.n2 + gamma*t.n3;
                normal = normal.normalized();

                Vec3 hit = Vec3(alpha*t.v1(0) + beta*t.v2(0) + gamma*t.v3(0),
                            alpha*t.v1(1) + beta*t.v2(1) + gamma*t.v3(1),
                            alpha*t.v1(2) + beta*t.v2(2) + gamma*t.v3(2));


                Vec3 lightPos = Vec3(10,10,10);
                Vec3 lightDirection = (lightPos - hit);
                lightDirection.normalize();

                float ambientFactor = 0.2f;
                float diffuseFactor = 0.5f;

                float scaleFactor = fmax(0, normal.dot(lightDirection))*diffuseFactor + ambientFactor;
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

int main(int, char**){

    //std::string meshName = "tet.obj";
    std::string meshName = "bunny.obj";

    SurfaceMesh mesh;
    if (!mesh.read(meshName)) {
        std::cerr << "Failed to read input mesh" << std::endl;
        return EXIT_FAILURE;
    }

    mesh.update_vertex_normals();
    //mesh.update_face_normals();

    auto vpoint = mesh.vertex_property<Vec3>("v:point");
    auto vnormal = mesh.vertex_property<Vec3>("v:normal");

    int wResolution = 800;
    int hResolution = 600;

    Image<Colour> image(hResolution, wResolution);
    Image<float> depth(hResolution, wResolution);

    // Set initial depth to large value
    for (int i = 0;i < depth.cols();i++) {
        for (int j = 0;j < depth.rows();j++) {
            depth(j,i) = std::numeric_limits<float>::max();
        }
    }

    /// Try varying projection parameters
    float fieldOfView = 80.0f; // degrees
    float near = 0.1f;
    float far = 5.0f;
    Mat4x4 projection = OpenGP::perspective(fieldOfView, (float)wResolution/(float)hResolution, near, far);
    //Mat4x4 projection = OpenGP::ortho(-1.5f*(float)wResolution/(float)hResolution, 1.5f*(float)wResolution/(float)hResolution, -1.5f, 1.5f, -1.5f, 1.5f);

    /// Try varying camera parameters
    // Vec3 eye = Vec3(-0.2f,0.4f,1);
    Vec3 eye = Vec3(0,0,1);
    Vec3 focus = Vec3(0,0,0);
    Vec3 up = Vec3(0,1,0);
    Mat4x4 view = OpenGP::lookAt(eye, focus, up);

    Mat4x4 model = Mat4x4::Identity();

    Mat4x4 mvp = projection * view * model;

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

        rasterize(t, image, depth);
    }

    bmpwrite("../../out.bmp", image);
    imshow(image);

    return EXIT_SUCCESS;
}
