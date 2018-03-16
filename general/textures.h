#pragma once
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include <SDL_image.h>

int MAX_WIDTH = 2048;
int MAX_HEIGHT = 2048;

void checkerBoardTexture(){
  const int width = 26;
  const int height = 26;
  float pixels[width*height*3];
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      int h = j*3;
      int w = i*3;
      if(j % 2 == 0 ^ i % 2 == 0){
        pixels[h*width+w] = 1.0f;
        pixels[h*width+w+1] = 1.0f;
        pixels[h*width+w+2] = 1.0f;
      }
      else{
        pixels[h*width+w] = 0.0f;
        pixels[h*width+w+1] = 0.0f;
        pixels[h*width+w+2] = 0.0f;
      }
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels);
}

void textureFromImage(const char* filename){
  SDL_Surface* Surface = IMG_Load(filename);
  if(!Surface){
    std::cout << "Failed to load surface";
  }
  int mode = GL_RGB;
  if(Surface->format->BytesPerPixel == 4) {
    mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, mode, Surface->w, Surface->h, 0, mode, GL_UNSIGNED_BYTE, Surface->pixels);
}
