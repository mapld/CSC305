#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl3w.h> // opengl wrapper

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>
#include <iostream>

#include "models.h"
#include "textures.h"
#include "gl_error_printing.h"
#include "project_init.h"
#include "terrain_shading.h"

const int SCREEN_HEIGHT= 720;
const int SCREEN_WIDTH=  1280;

GLuint gProgramID = 0;
GLuint gMatrixID;
GLuint gTex;
GLint gTexAttrib = -1;
GLint gVertAttrib = -1;
GLuint gVAO = -1;

struct TriangleStripMesh{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uv_coords;
  std::vector<int> indicies;
  GLuint VBO;
  GLuint IBO;
};

struct Scene{
  TriangleStripMesh plane_mesh;
  Model model;
};

void loadTriangleStripIntoGL(TriangleStripMesh& mesh){

}

int initScene(Scene& scene){
  glGenVertexArrays(1, &gVAO);
  glBindVertexArray(gVAO);

  gProgramID = loadTerrainShaders();

  gVertAttrib = glGetAttribLocation(gProgramID, "LVertexPos3D");
  if(gVertAttrib == -1){
    printf( "LVertexPos3D is not a valid glsl program variable!\n" );
    return -1;
  }
  gTexAttrib = glGetAttribLocation(gProgramID, "TexCoord");
  if(gTexAttrib == -1){
    printf( "TexCoord is not a valid glsl program variable!\n" );
    return -1;
  }

  gTex = loadAndBindNewTexture();
  // textureFromImage(textureFilename.c_str());
  checkerBoardTexture();

  scene.model = createCubeModel();
  loadModelToGL(scene.model);

  gMatrixID = glGetUniformLocation(gProgramID, "MVP");

  return 0;
}

int renderScene(Scene& scene, glm::vec3 camera){
  glUseProgram(gProgramID);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(
                               camera, // Camera is at (4,3,3), in World Space
                               glm::vec3(0,0,0), // looks at the origin
                               glm::vec3(0,1,0)
                               );

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 mvp = projection * view * model;


  RenderVars rv;
  rv.vertAttrib = gVertAttrib;
  rv.matrixID = gMatrixID;
  rv.otherAttrib = gTexAttrib;

  glEnableVertexAttribArray( gVertAttrib );

  renderModel(scene.model, mvp, rv);

  //Disable vertex position
  glDisableVertexAttribArray( gVertAttrib );

  //Unbind program
  glUseProgram( NULL );

  return 0;
}

int main(int, char**){
  SdlHandle sdl_handle = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
  if(sdl_handle.code < 0){
    std::cout << "Failed to init SDL\n";
    return sdl_handle.code;
  }
  SDL_Window* window = sdl_handle.window;

  initGL();
  initImGui(sdl_handle);

  // Actual scene setup
  glm::vec3 camera = glm::vec3(4,3,4);
  Scene scene;
  initScene(scene);

  bool done = false;
  while (!done){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
      ImGui_ImplSdlGL3_ProcessEvent(&event);
      if (event.type == SDL_QUIT){
        done = true;
      }
    }

    ImGui_ImplSdlGL3_NewFrame(window);

    // Any imgui buttons / widgets go here
    {
    }

    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0,0.5f,0.8f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene(scene, camera);

    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  return 0;
}
