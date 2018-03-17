#pragma once
#include "scene.h"
#include "bezier.h"

float lerp(float start, float end, float t){
  return start + (end-start)*t;
}

void applyAnimations(Scene& scene, float seconds_passed){
  for(ModelInst& modelInst : scene.modelInstances){
    modelInst.transform = modelInst.baseTransform;
  }

  for(LinearScaleAnimation& animation: scene.animations.scaling){

    AnimationBase base = animation.animation_base;

    float animation_time = base.animation_total_time;
    float cT = fmod(seconds_passed, animation_time) / animation_time;

    float scale_factor = lerp(animation.start_scale, animation.end_scale, cT);

    int model_inst = base.model_inst;
    scene.modelInstances[model_inst].transform.scale *= glm::vec3(scale_factor, scale_factor, scale_factor);
  }

  for(WingFlapAnimation& animation: scene.animations.wings){
    AnimationBase base = animation.animation_base;

    float total_animation_time = base.animation_total_time;
    float curAnimationTime = fmod(seconds_passed, total_animation_time);

    float leftAngle;
    float wingAdj;
    if(curAnimationTime >= total_animation_time / 2.0f){
      float animationTimeAdj = (curAnimationTime - total_animation_time/2.0f) / (total_animation_time/2.0f);
      leftAngle = animation.bottom_angle + animationTimeAdj*(animation.top_angle - animation.bottom_angle);
      wingAdj = animation.wing_adj_factor - 2*(animationTimeAdj* animation.wing_adj_factor);
    }
    else{
      float animationTimeAdj = curAnimationTime / (total_animation_time/2.0f);
      leftAngle = animation.top_angle + animationTimeAdj*(animation.bottom_angle - animation.top_angle);
      wingAdj = -1*animation.wing_adj_factor + 2*(animationTimeAdj* animation.wing_adj_factor);
    }

    int model_inst = base.model_inst;
    if(!animation.left){
      leftAngle = -leftAngle;
    }
    scene.modelInstances[model_inst].transform.position += glm::vec3(0,wingAdj,0);
    scene.modelInstances[model_inst].transform.rotation += leftAngle;
  }

  for(BezierCurveAnimation& animation: scene.animations.bezier){
    AnimationBase base = animation.animation_base;
    if(base.model_inst == -1){
      continue;
    }
    float total_animation_time = base.animation_total_time;
    float cT = fmod(seconds_passed, total_animation_time) / total_animation_time;

    glm::vec3 location = getBezierPoint(animation, cT);
    int model_inst = base.model_inst;

    scene.modelInstances[model_inst].transform.position += location;
  }
};
