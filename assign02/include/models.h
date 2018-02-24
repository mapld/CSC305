#pragma once
#include <vector>
#include <glm/common.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

struct Model{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvCoords;
  std::vector<glm::vec3> triangleIndicies;
};

void writeModelToObj(Model& model, char* filename){
  std::ofstream obj_file(filename);
  for(glm::vec3 vert : model.vertices){
    obj_file << "v " << vert.x << " " << vert.y << " " << vert.z << "\n";
  }
  for(glm::vec2 texCoord : model.uvCoords){
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

Model loadModelFromObj(char* filename){
  Model model;

  std::ifstream obj_file(filename);
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
      model.triangleIndicies.push_back(glm::vec3(x,y,z));
    }

  }
  return model;
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
