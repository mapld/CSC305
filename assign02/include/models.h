#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define _USE_MATH_DEFINES

struct Model{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvCoords;
  std::vector<glm::vec3> triangleIndicies;
};

void writeModelToObj(Model& model, std::string filename){
  std::ofstream obj_file(filename.c_str());
  for(int i = 0; i < model.vertices.size(); i++){
    glm::vec3 vert = model.vertices[i];
    obj_file << "v " << vert.x << " " << vert.y << " " << vert.z << "\n";
  }
  for(int i = 0; i < model.uvCoords.size(); i++){
    glm::vec2 texCoord = model.uvCoords[i];
    obj_file << "vt " << texCoord.x << " " << texCoord.y << "\n";
  }

  for(glm::vec3 t : model.triangleIndicies){
    int tx = (int)t.x + 1;
    int ty = (int)t.y + 1;
    int tz = (int)t.z + 1;
    obj_file << "f "
             << tx << "/" << tx << " "
             << ty << "/" << ty << " "
             << tz << "/" << tz << " "
             << "\n";
  }
}

void loadModelFromObj(std::string filename, Model& model){
  std::ifstream obj_file(filename.c_str());
  if(!obj_file.is_open()){
    std::cout << "ERROR: failed to read from file " << filename << "\n";
    return;
  }
  std::string line;
  while(std::getline(obj_file,line)){
    std::istringstream iss(line);

    std::string lineType;
    iss >> lineType;

    if(lineType == "v"){
      float x,y,z;
      iss >> x;
      iss >> y;
      iss >> z;
      model.vertices.push_back(glm::vec3(x,y,z));
    }

    if(lineType == "vt"){
      float x,y;
      iss >> x;
      iss >> y;
      model.uvCoords.push_back(glm::vec2(x,y));
    }

    if(lineType == "f"){
      int x, y, z;
      int cX, cY, cZ;
      char c;
      iss >> x;
      iss >> c;
      iss >> cX;
      iss >> y;
      iss >> c;
      iss >> cY;
      iss >> z;
      iss >> c;
      iss >> cZ;
      x -= 1;
      y -= 1;
      z -= 1;
      model.triangleIndicies.push_back(glm::vec3(x,y,z));
    }

  }
}

Model createCubeModel(){
  Model model;
  model.vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
  model.vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
  model.vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));

  model.vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
  model.vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
  model.vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
  model.vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));

  model.triangleIndicies.push_back(glm::vec3(0, 1, 2));
  model.triangleIndicies.push_back(glm::vec3(0, 2, 3));
  model.triangleIndicies.push_back(glm::vec3(4, 5, 6));
  model.triangleIndicies.push_back(glm::vec3(4, 6, 7));
  model.triangleIndicies.push_back(glm::vec3(3, 2, 7));
  model.triangleIndicies.push_back(glm::vec3(2, 6, 7));
  model.triangleIndicies.push_back(glm::vec3(1, 0, 5));
  model.triangleIndicies.push_back(glm::vec3(0, 4, 5));
  model.triangleIndicies.push_back(glm::vec3(1, 2, 5));
  model.triangleIndicies.push_back(glm::vec3(2, 5, 6));
  model.triangleIndicies.push_back(glm::vec3(0, 3, 4));
  model.triangleIndicies.push_back(glm::vec3(3, 4, 7));

  return model;
}

Model createCylinderModel(glm::vec3 center, float radius, float height, int detail){
  Model model;

  int pi = glm::pi<float>();

  float divisions = float(detail);

  for(int i = 0; i < detail; i++){
    //top
    model.vertices.push_back(glm::vec3(
                                       radius * glm::sin(2*pi *i/divisions),
                                       center.y + height/2,
                                       radius * glm::cos(2*pi* i/divisions))
                                       );

    float u = i/(divisions);
    float v = 1;
    model.uvCoords.push_back(glm::vec2(u,v));
    //bottom
    model.vertices.push_back(glm::vec3(
                                       radius * glm::sin(2*pi *i/divisions),
                                       center.y - height/2,
                                       radius * glm::cos(2*pi* i/divisions))
                             );
    v = 0;
    model.uvCoords.push_back(glm::vec2(u,v));
  }

  // extra vertex for edge
  model.vertices.push_back(model.vertices[0]);
  model.vertices.push_back(model.vertices[1]);
  model.uvCoords.push_back(glm::vec2(1,1));
  model.uvCoords.push_back(glm::vec2(1,0));

  // top cap center
  model.vertices.push_back(center + glm::vec3(0,height/2,0));
  model.uvCoords.push_back(glm::vec2(1,1));

  // bottom cap center
  model.vertices.push_back(center + glm::vec3(0,-height/2,0));
  model.uvCoords.push_back(glm::vec2(1,0));

  int total_points = model.vertices.size();
  int top_cap_i = total_points-2;
  int bot_cap_i = total_points-1;
  for(int i = 0; i < detail; i++){
    int cur = (i*2);// % total_points;
    int down = (cur+1);// % total_points;
    int right = (cur+2);// % total_points;
    int down_right = (cur+3);// % total_points;
    model.triangleIndicies.push_back(glm::vec3(cur, down, right));
    model.triangleIndicies.push_back(glm::vec3(down, down_right, right));
    model.triangleIndicies.push_back(glm::vec3(cur,right,top_cap_i));
    model.triangleIndicies.push_back(glm::vec3(down,down_right,bot_cap_i));
  }
  return model;
}

Model createSphereModel(glm::vec3 center, float radius, int n_latitude, int n_longitude){
  Model model;
  int pi = glm::pi<float>();

  float n_lat = float(n_latitude);
  float n_long = float(n_longitude);

  for(int p = 0; p < n_lat; p++){
    float u = 0.0f;
    float v = 0.0f;
    for(int q = 0; q < n_long; q++){
      glm::vec3 vert = glm::vec3(
                          center.x + glm::sin(pi * p/n_lat) * glm::cos(2*pi*q/n_long) * radius,
                          center.y + glm::sin(pi * p / n_lat) * glm::sin(2*pi*q/n_long) * radius,
                          center.z + cos(pi * p / n_lat) * radius
                                );
      model.vertices.push_back(vert);

      // u = (vert.z - center.z / radius) / (2 * pi);
      // v = pi - (atan2(vert.y - center.y, vert.x - center.x)) / pi;
      glm::vec3 n = glm::normalize(vert - center);
      u = atan2(n.x, n.z) / (2*pi) + 0.5;
      v = n.y * 0.5 + 0.5;
      model.uvCoords.push_back(glm::vec2(u,v));
    }
    // model.vertices.push_back(model.vertices[p*(n_long+1)]);
    // model.uvCoords.push_back(glm::vec2(u,1));
  }

  int total_points = model.vertices.size();
  for(int p = 0; p < n_lat; p++){
    for(int q = 0; q < n_long; q++){
      int cur = int(p * (n_long) + q) % total_points;
      int right = (cur + 1) % total_points;
      int up = int(cur + n_long) % total_points;
      int up_right = (up + 1) % total_points;

      glm::vec3 t1 = glm::vec3(cur, up_right, right);
      glm::vec3 t2 = glm::vec3(cur, up, up_right);
      model.triangleIndicies.push_back(t1);
      model.triangleIndicies.push_back(t2);
    }
  }

  return model;
}
