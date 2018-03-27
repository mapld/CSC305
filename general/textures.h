#pragma once
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include <SDL_image.h>
#include <string>

int MAX_WIDTH = 2048;
int MAX_HEIGHT = 2048;


GLuint loadAndBindNewTexture(){
  //texture
  GLuint tex = 0;

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

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

void solidTexture(float r=0.0f, float g=0.0f, float b=0.0f){
  const int width = 26;
  const int height = 26;
  float pixels[width*height*3];
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      int h = j*3;
      int w = i*3;
      pixels[h*width+w] = r;
      pixels[h*width+w+1] = g;
      pixels[h*width+w+2] = b;
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

void textureCubeMapFromImages(){
  const std::string skyList[] = {"miramar_ft", "miramar_bk",  "miramar_up","miramar_dn", "miramar_rt", "miramar_lf"};

  for(int i=0; i < 6; ++i) {
    std::cout << "testi:" << i << "\n";
    const char* filename = ("Textures/" + skyList[i] + ".png").c_str();
    SDL_Surface* Surface = IMG_Load(filename);
    if(!Surface){
      std::cout << "Failed to load surface";
    }
    int mode = GL_RGB;
    if(Surface->format->BytesPerPixel == 4) {
      mode = GL_RGBA;
    }

    std::cout << "testi2\n";
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, mode, Surface->w, Surface->h, 0, mode, GL_UNSIGNED_BYTE, Surface->pixels);
    std::cout << "testi3\n";
  }
}
