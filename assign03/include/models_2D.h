#pragma once
#include "models.h"

Model createRectModel(float width, float height){
  Model model;
  model.vertices.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
  model.vertices.push_back(glm::vec3(0.0f, height, 0.5f));
  model.vertices.push_back(glm::vec3(width, height, 0.5f));
  model.vertices.push_back(glm::vec3(width, 0.0f, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));

  model.triangleIndicies.push_back(glm::vec3(0, 1, 2));
  model.triangleIndicies.push_back(glm::vec3(0, 2, 3));

  return model;
}

void addCircle(Model& model, glm::vec3 center, float radius){
  float nVerts = 360;
  float pi = glm::pi<float>();
  int centerI = model.vertices.size();
  int curI = centerI;
  model.vertices.push_back(center);
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  for(float theta = 0.0f; theta < 2*pi; theta += (2*pi/nVerts)){
    curI++;
    model.vertices.push_back(glm::vec3(center.x + radius * glm::cos(theta),
                                       center.y + radius * glm::sin(theta),
                                       0.5f));
    model.uvCoords.push_back(glm::vec2(theta/(2*pi),0.0f));
    if(theta > 0.00001f){
      model.triangleIndicies.push_back(glm::vec3(centerI, curI, curI-1));
    }
  }
  model.triangleIndicies.push_back(glm::vec3(centerI, centerI+1, curI));
}

void addRect(Model& model, glm::vec3 topLeft, float width, float height){
  int curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(topLeft.x, topLeft.y, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x, topLeft.y + height, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x + width, topLeft.y + height, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x + width, topLeft.y, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));

  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+1, curI+2));
  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+2, curI+3));
}

Model createBatWing(){
  Model model;
  addRect(model, glm::vec3(-0.1f,-0.02f,0.5f), 0.2f, 0.04f);
  return model;
}

Model createBatBody(){
  Model model;

  int curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(-0.045f, -0.03f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.045f, 0.09f, 0.5f));
  model.vertices.push_back(glm::vec3(0.045f, 0.09f, 0.5f));
  model.vertices.push_back(glm::vec3(0.045f, -0.03f, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));

  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+1, curI+2));
  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+2, curI+3));

  // head
  addCircle(model, glm::vec3(0,-0.04,0.5f), 0.05);

  // ears
  curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(-0.025f, -0.11f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.040f, -0.07f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.010f, -0.07f, 0.5f));
  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.triangleIndicies.push_back(glm::vec3(curI, curI+1, curI+2));

  curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(0.025f, -0.11f, 0.5f));
  model.vertices.push_back(glm::vec3(0.040f, -0.07f, 0.5f));
  model.vertices.push_back(glm::vec3(0.010f, -0.07f, 0.5f));
  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.triangleIndicies.push_back(glm::vec3(curI, curI+1, curI+2));

  return model;
}
