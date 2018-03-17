#pragma once
#include <vector>
#include "models.h"

// multiple animations:
// contain details of animation
// start with simplest animation, linear scaling
struct AnimationBase{
  int model_inst;
  float animation_total_time;
};
struct LinearScaleAnimation{
  AnimationBase animation_base;
  float start_scale;
  float end_scale;
};
struct WingFlapAnimation{
  AnimationBase animation_base;
  float top_angle;
  float bottom_angle;
  float wing_adj_factor = 0.05;
  bool left;
};
struct BezierCurveAnimation{
  AnimationBase animation_base;
  Model pointModel;
  std::vector<glm::vec3> points;
  std::vector<glm::vec3> linePoints;
  GLuint lineVBO = 0;
  GLuint lineIBO = 0;
  glm::mat4 lineMVP;
};

struct Animations{
  std::vector<WingFlapAnimation> wings;
  std::vector<BezierCurveAnimation> bezier;
  std::vector<LinearScaleAnimation> scaling;
};

