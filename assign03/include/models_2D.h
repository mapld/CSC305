#pragma once
#include "models.h"
#include "animation_defs.h"
#include "bezier.h"

static glm::vec3 BLACK = glm::vec3(0.0,0.0,0.0);
static glm::vec3 RED = glm::vec3(0.8f,0.2f,0.2f);

Model createRectModel(float width, float height, glm::vec3 color = BLACK){
  Model model;
  model.vertices.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
  model.vertices.push_back(glm::vec3(0.0f, height, 0.5f));
  model.vertices.push_back(glm::vec3(width, height, 0.5f));
  model.vertices.push_back(glm::vec3(width, 0.0f, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));
  model.colors.push_back(color);
  model.colors.push_back(color);
  model.colors.push_back(color);
  model.colors.push_back(color);

  model.triangleIndicies.push_back(glm::vec3(0, 1, 2));
  model.triangleIndicies.push_back(glm::vec3(0, 2, 3));

  return model;
}

void addCircle(Model& model, glm::vec3 center, float radius, glm::vec3 color = BLACK){
  float nVerts = 360;
  float pi = glm::pi<float>();
  int centerI = model.vertices.size();
  int curI = centerI;
  model.vertices.push_back(center);
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.colors.push_back(color);
  for(float theta = 0.0f; theta < 2*pi; theta += (2*pi/nVerts)){
    curI++;
    model.vertices.push_back(glm::vec3(center.x + radius * glm::cos(theta),
                                       center.y + radius * glm::sin(theta),
                                       0.5f));
    model.uvCoords.push_back(glm::vec2(theta/(2*pi),0.0f));
    model.colors.push_back(color);
    if(theta > 0.00001f){
      model.triangleIndicies.push_back(glm::vec3(centerI, curI, curI-1));
    }
  }
  model.triangleIndicies.push_back(glm::vec3(centerI, centerI+1, curI));
}

void addRect(Model& model, glm::vec3 topLeft, float width, float height, glm::vec3 color = BLACK){
  int curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(topLeft.x, topLeft.y, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x, topLeft.y + height, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x + width, topLeft.y + height, 0.5f));
  model.vertices.push_back(glm::vec3(topLeft.x + width, topLeft.y, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));
  model.colors.push_back(color);
  model.colors.push_back(color);
  model.colors.push_back(color);
  model.colors.push_back(color);

  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+1, curI+2));
  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+2, curI+3));
}

void addFilledCurve(Model & model, BezierCurveAnimation& curve, glm::vec3 base){
  addCurveFromPoints(curve);

  int baseI = model.vertices.size();
  // model.vertices.push_back(base); // lower left corner of rect
  // model.colors.push_back(BLACK);
  int curI = baseI;
  for(glm::vec3 point : curve.linePoints){
    model.vertices.push_back(point);
    model.vertices.push_back(glm::vec3(point.x, base.y, point.z));
    model.colors.push_back(BLACK);
    model.colors.push_back(BLACK);
    curI +=2;
    if(curI - baseI > 2){
      model.triangleIndicies.push_back(glm::vec3(curI-4, curI-3, curI-2));
      model.triangleIndicies.push_back(glm::vec3(curI-3, curI-2, curI-1));
    }
  }
}

Model createBatWing(){
  Model model;
  addRect(model, glm::vec3(-0.1f,-0.02f,0.5f), 0.2f, 0.04f);

  BezierCurveAnimation curve;
  curve.animation_base.model_inst = -1;
  curve.points.push_back(glm::vec3(-0.1f,0.04f,0.05f));
  curve.points.push_back(glm::vec3(-0.065f,0.035f,0.05f));
  curve.points.push_back(glm::vec3(-0.035f,0.025f,0.05f));
  curve.points.push_back(glm::vec3(0.0f,0.025f,0.05f));
  addFilledCurve(model, curve, glm::vec3(-0.1,0.02f,0.5f));

  BezierCurveAnimation right_curve;
  curve.animation_base.model_inst = -1;
  right_curve.points.push_back(glm::vec3(0.0f,0.025f,0.05f));
  right_curve.points.push_back(glm::vec3(0.035f,0.025f,0.05f));
  right_curve.points.push_back(glm::vec3(0.065f,0.035f,0.05f));
  right_curve.points.push_back(glm::vec3(0.1f,0.04f,0.05f));
  addFilledCurve(model, right_curve, glm::vec3(0.1,0.02f,0.5f));

  BezierCurveAnimation top_curve;
  curve.animation_base.model_inst = -1;
  top_curve.points.push_back(glm::vec3(0.1f, -0.02f,0.05f));
  top_curve.points.push_back(glm::vec3(0.05f, -0.04f,0.05f));
  top_curve.points.push_back(glm::vec3(-0.05f, -0.04f,0.05f));
  top_curve.points.push_back(glm::vec3(-0.1f, -0.02f,0.05f));
  addFilledCurve(model, top_curve, glm::vec3(0.0,-0.02f,0.5f));

  return model;
}

Model createBatBody(){
  Model model;

  int curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(-0.045f, -0.03f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.045f, 0.04f, 0.5f));
  model.vertices.push_back(glm::vec3(0.045f, 0.04f, 0.5f));
  model.vertices.push_back(glm::vec3(0.045f, -0.03f, 0.5f));

  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,0.0f));
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);

  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+1, curI+2));
  model.triangleIndicies.push_back(glm::vec3(curI+0, curI+2, curI+3));

  // head
  addCircle(model, glm::vec3(0,-0.04,0.5f), 0.05);

  //eyes
  addCircle(model, glm::vec3(-0.02,-0.05,0.6f), 0.0075, RED);
  addCircle(model, glm::vec3(0.02,-0.05,0.6f), 0.0075, RED);

  // ears
  curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(-0.025f, -0.11f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.040f, -0.07f, 0.5f));
  model.vertices.push_back(glm::vec3(-0.010f, -0.07f, 0.5f));
  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.triangleIndicies.push_back(glm::vec3(curI, curI+1, curI+2));

  curI = model.vertices.size();
  model.vertices.push_back(glm::vec3(0.025f, -0.11f, 0.5f));
  model.vertices.push_back(glm::vec3(0.040f, -0.07f, 0.5f));
  model.vertices.push_back(glm::vec3(0.010f, -0.07f, 0.5f));
  model.uvCoords.push_back(glm::vec2(0.0f,0.0f));
  model.uvCoords.push_back(glm::vec2(0.0f,1.0f));
  model.uvCoords.push_back(glm::vec2(1.0f,1.0f));
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.colors.push_back(BLACK);
  model.triangleIndicies.push_back(glm::vec3(curI, curI+1, curI+2));

  return model;
}
