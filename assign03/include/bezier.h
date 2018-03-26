#pragma once
#include "scene.h"

glm::vec3 getBezierPoint(const BezierCurveAnimation& animation, float t){
  std::vector<glm::vec3> controlPoints = animation.points;
  return powf((1-t), 3)*controlPoints[0]
    + 3.0f * powf((1-t),2) * t * controlPoints[1]
    + 3.0f * (1 - t) * powf(t,2) * controlPoints[2]
    + powf(t,3) * controlPoints[3];
}

// Takes 4 points and creates a larger series of points that can be drawn as a bezier curve
void addCurveFromPoints(BezierCurveAnimation& animation){
  animation.linePoints.clear();
  float NUM_LINES = 400;
  for(float t = 0; t < 1; t+= 1.0f/NUM_LINES){
    glm::vec3 bezier_point = getBezierPoint(animation, t);
    animation.linePoints.push_back(bezier_point);
  }
}
