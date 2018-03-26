#pragma once

#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl3w.h> // opengl wrapper


struct SdlHandle{
  int code;
  SDL_Window* window;
};

SdlHandle initSDL(int screen_width, int screen_height){
  SdlHandle handle;
  handle.code = 0;
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
    std::cout << "Error: " << SDL_GetError() << "\n";
    handle.code = -1;
    return handle;
  }

  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if(!(IMG_Init(imgFlags) & imgFlags)){
    std::cout << "Image Error: " << SDL_GetError() << "\n";
    handle.code = -1;
    return handle;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);


  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_Window *window = SDL_CreateWindow("CSC305 Assignment 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  handle.window = window;
  return handle;
}

void initGL(){
  gl3wInit();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void initImGui(SdlHandle& sdl_handle){

  // ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplSdlGL3_Init(sdl_handle.window);

  // ImGui style
  ImGui::StyleColorsDark();
}

