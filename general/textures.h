#pragma once
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <glm/glm.hpp>
#include <time.h>

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

inline float lerp(float x, float y, float t) {
  return x + (t * (y-x));
}

inline float fade(float t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float rand01() {
  return ((float) std::rand())/((float) RAND_MAX);
}

float* perlin2D(const int width, const int height, const int period=64);

void noiseTexture(){
  const int width = 512;
  const int height = 512;

  ///--- fBm parameters
  float H = 1.1f;
  float lacunarity = 2.1f;
  float offset = 0.16f;
  const int octaves = 5;

  // int perlinWidth = width*(pow(lacunarity,octaves)+1);
  // int perlinHeight = height*(pow(lacunarity,octaves)+1);
  int perlinWidth = width;
  int perlinHeight = height;
  float *perlin_data = perlin2D(perlinWidth, perlinHeight, 128);

  ///--- Initialize to 0s
  float *noise_data = new float[width*height];
  for (int i = 0; i < width; ++ i) {
    for (int j = 0; j < height; ++ j) {
      // noise_data[i+j*height] = perlin_data[i+j*height];
      noise_data[i+j*width] = 0;
    }
  }

  ///--- Precompute exponent array
  float *exponent_array = new float[octaves];
  float f = 1.0f;
  for (int i = 0; i < octaves; ++i) {
    /// TODO: Implement this step according to Musgraves paper on fBM
    exponent_array[i] = pow(f,-H);
    f *= lacunarity;
  }

  for (int i = 0; i < width; ++ i) {
    for (int j = 0; j < height; ++ j) {
      int I = i;
      int J = j;
      for(int k = 0; k < octaves; ++k) {
        /// TODO: Get perlin noise at I,J, add offset, multiply by proper term and add to noise_data
        // p_data_ind = (I)+(J)*perlinWidth;
        int p_data_ind = (I % perlinWidth) +(J % perlinHeight)*perlinWidth;
        noise_data[i+j*width] += exponent_array[k]*(perlin_data[p_data_ind] + offset);

        ///--- Point to sample at next octave
        I *= (int) lacunarity;
        J *= (int) lacunarity;
      }
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, noise_data);
  delete[] noise_data;
  delete perlin_data;
}

float* perlin2D(const int width, const int height, const int period) {
    srand (time(NULL));

    ///--- Precompute random gradients
    float *gradients = new float[width*height*2];
    auto sample_gradient = [&](int i, int j) {
        float x = gradients[2*(i+j*height)];
        float y = gradients[2*(i+j*height)+1];
        return glm::vec2(x,y);
    };

    for (int i = 0; i < width; ++ i) {
        for (int j = 0; j < height; ++ j) {
            float angle = rand01();
            gradients[2*(i+j*height)] = cos(2 * angle * M_PI);
            gradients[2*(i+j*height)+1] = sin(2 * angle * M_PI);
        }
    }

    ///--- Perlin Noise parameters
    float frequency = 1.0f / period;

    float *perlin_data = new float[width*height];
    for (int i = 0; i < width; ++ i) {
        for (int j = 0; j < height; ++ j) {

            ///--- Integer coordinates of corners
            int left = (i / period) * period;
            int right = (left + period) % width;
            int top = (j / period) * period;
            int bottom = (top + period) % height;

            ///--- local coordinates [0,1] within each block
            float dx = (i - left) * frequency;
            float dy = (j - top) * frequency;

            ///--- Fetch random vectors at corners
            glm::vec2 topleft = sample_gradient(left, top);
            glm::vec2 topright = sample_gradient(right, top);
            glm::vec2 bottomleft = sample_gradient(left, bottom);
            glm::vec2 bottomright = sample_gradient(right, bottom);

            ///--- Vector from each corner to pixel center
            glm::vec2 a(dx,      -dy); // topleft
            glm::vec2 b(dx-1,    -dy); // topright
            glm::vec2 c(dx,      1 - dy); // bottomleft
            glm::vec2 d(dx-1,    1 - dy); // bottomright

            float s = glm::dot(topleft,a);
            float t = glm::dot(topright,b);
            float u = glm::dot(bottomleft,c);
            float v = glm::dot(bottomright,d);

            float st = lerp(s,t,fade(dx));
            float uv = lerp(u,v,fade(dx));

            float noise = lerp(st,uv,fade(dy));

            perlin_data[i+j*height] = noise;
        }
    }

    delete[] gradients;
    return perlin_data;
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
    const char* filename = ("Textures/" + skyList[i] + ".png").c_str();
    SDL_Surface* Surface = IMG_Load(filename);
    if(!Surface){
      std::cout << "Failed to load surface";
    }
    int mode = GL_RGB;
    if(Surface->format->BytesPerPixel == 4) {
      mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, mode, Surface->w, Surface->h, 0, mode, GL_UNSIGNED_BYTE, Surface->pixels);
  }
}
