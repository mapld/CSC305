#pragma once
#include <vector>
#include "models_2D.h"
#include "animation_defs.h"

struct Transform{
  glm::vec3 position;
  glm::vec3 scale;
  float rotation;
};

struct ModelInst{
  int modelNum;
  int parent;
  Transform baseTransform;
  Transform transform;
};

struct Scene{
  std::vector<Model> models;

  // static scene where nothing gets deleted, we give out indexes to this array which are accessed directly
  std::vector<ModelInst> modelInstances;

  // result of transform function
  std::vector<glm::mat4> mvps;

  Animations animations;

  // to be moved to animation struct

  bool actionDown = false;
  int pointUnderMouse = -1;
  int curveUnderMouse = -1;

  bool editMode = false;
};


int addModelToScene(Scene& scene, int modelNum, int parent = -1, glm::vec3 position = glm::vec3(0,0,0)){
  ModelInst modelInstance;
  modelInstance.modelNum = modelNum;
  modelInstance.baseTransform.position = position;
  modelInstance.parent = parent;
  modelInstance.baseTransform.scale = glm::vec3(1,1,1);
  modelInstance.baseTransform.rotation = 0;
  scene.modelInstances.push_back(modelInstance);
  return scene.modelInstances.size() - 1;
}

int addLinearScaleAnimationToScene(Scene& scene, int model_inst, float time = 2.0f, float start_scale = 0.2f, float end_scale = 1.2f){
  AnimationBase base = {model_inst, time};
  LinearScaleAnimation animation = {base, start_scale, end_scale};
  scene.animations.scaling.push_back(animation);
  return scene.animations.scaling.size() - 1;
}

int addWingAnimation(Scene& scene, int model_inst, bool left, float time = 2.0f, float top_angle = 0.5f, float bottom_angle = -0.5f){
  AnimationBase base = {model_inst, time};
  WingFlapAnimation animation;
  animation.animation_base = base;
  animation.top_angle = top_angle;
  animation.bottom_angle = bottom_angle;
  animation.left = left;
  scene.animations.wings.push_back(animation);
  return scene.animations.wings.size() - 1;
}

int addBezierCurveAnimation(Scene& scene, BezierCurveAnimation animation){
  scene.animations.bezier.push_back(animation);
  return scene.animations.bezier.size() - 1;
}
