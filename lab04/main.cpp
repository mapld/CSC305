#include "OpenGP/GL/Eigen.h"
#include "OpenGP/GL/glfw_helpers.h"
#include "Mesh/Mesh.h"

using namespace OpenGP;

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

const int SUN_ROT_PERIOD = 30;        
const int EARTH_ROT_PERIOD = 4;       
const int MOON_ROT_PERIOD = 8;       
const int EARTH_ORBITAL_PERIOD = 10; 
const int MOON_ORBITAL_PERIOD = 5;   
const int SPEED_FACTOR = 1;
    
Mesh sun;
Mesh earth;
Mesh moon;

void init();

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime();

    //TODO: Set up the transform hierarchies for the three objects!
    float freq = M_PI*time_s*SPEED_FACTOR;

    // **** Sun transform
    Transform sun_M = Transform::Identity();
    float scale_t = 0.01*std::sin(freq);
    sun_M *= Eigen::AlignedScaling3f(0.2f + scale_t, 0.2f + scale_t, 1.0f);
    sun_M *= Eigen::AngleAxisf(-freq/SUN_ROT_PERIOD, Eigen::Vector3f::UnitZ());

    // **** Earth transform
    Transform earth_M = Transform::Identity();
    float x_earth_orbit = 0.7*std::cos(-freq/EARTH_ORBITAL_PERIOD);
    float y_earth_orbit = 0.5*std::sin(-freq/EARTH_ORBITAL_PERIOD);

    earth_M *= Eigen::Translation3f(x_earth_orbit, y_earth_orbit, 0.0);
    Transform earth_M_prespin = earth_M;
    earth_M *= Eigen::AlignedScaling3f(0.08f, 0.08f, 1.0f);
    earth_M *= Eigen::AngleAxisf(-freq/EARTH_ROT_PERIOD, Eigen::Vector3f::UnitZ());


    // **** Moon transform
    Transform moon_M = earth_M_prespin;
    moon_M *= Eigen::AngleAxisf(freq/MOON_ORBITAL_PERIOD, Eigen::Vector3f::UnitZ());
    moon_M *= Eigen::Translation3f(0.2, 0.0, 0.0);
    moon_M *= Eigen::AngleAxisf(-freq/MOON_ROT_PERIOD, Eigen::Vector3f::UnitZ());
    moon_M *= Eigen::AlignedScaling3f(0.05f, 0.05f, 1.0f);

    // draw the sun, the earth and the moon
    sun.draw(sun_M.matrix());
    earth.draw(earth_M.matrix());
    moon.draw(moon_M.matrix());
}

int main(int, char**){
    glfwInitWindowSize(512, 512);
    glfwMakeWindow("Planets");
    glfwDisplayFunc(display);
    init();
    glfwMainLoop();
    return EXIT_SUCCESS;
}

void init(){
    glClearColor(1.0f,1.0f,1.0f, 1.0 );

    // Enable alpha blending so texture backgroudns remain transparent
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sun.init();
    earth.init();
    moon.init();

    std::vector<OpenGP::Vec3> quadVert;
    quadVert.push_back(OpenGP::Vec3(-1.0f, -1.0f, 0.0f));
    quadVert.push_back(OpenGP::Vec3(1.0f, -1.0f, 0.0f));
    quadVert.push_back(OpenGP::Vec3(1.0f, 1.0f, 0.0f));
    quadVert.push_back(OpenGP::Vec3(-1.0f, 1.0f, 0.0f));
    std::vector<unsigned> quadInd;
    quadInd.push_back(0);
    quadInd.push_back(1);
    quadInd.push_back(2);
    quadInd.push_back(0);
    quadInd.push_back(2);
    quadInd.push_back(3);
    sun.loadVertices(quadVert, quadInd);
    earth.loadVertices(quadVert, quadInd);
    moon.loadVertices(quadVert, quadInd);

    std::vector<OpenGP::Vec2> quadTCoord;
    quadTCoord.push_back(OpenGP::Vec2(0.0f, 0.0f));
    quadTCoord.push_back(OpenGP::Vec2(1.0f, 0.0f));
    quadTCoord.push_back(OpenGP::Vec2(1.0f, 1.0f));
    quadTCoord.push_back(OpenGP::Vec2(0.0f, 1.0f));
    sun.loadTexCoords(quadTCoord);
    earth.loadTexCoords(quadTCoord);
    moon.loadTexCoords(quadTCoord);

    sun.loadTextures("sun.png");
    moon.loadTextures("moon.png");
    earth.loadTextures("earth.png");
}
