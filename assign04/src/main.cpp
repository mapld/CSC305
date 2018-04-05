#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl3w.h> // opengl wrapper

#include <glm/glm.hpp>
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
#include "triangle_strip_mesh.h"

const int SCREEN_HEIGHT= 720;
const int SCREEN_WIDTH=  1280;

GLuint gTerrainProgramID = 0;
GLuint gMatrixID = 0;
GLuint gCameraUniform = 0;
GLuint gTex = 0;
GLint gUvCoordAttrib = -1;
GLint gVertAttrib = -1;
GLuint gVAO = -1;

struct SkyboxProgram{
  GLuint program_id = 0;
  GLuint view_id = 0;
  GLuint projection_id = 0;
  GLint vert_attrib = -1;
  GLuint texture = 0;
  GLuint VBO = 0;
  GLuint IBO = 0;
  int box_size = 0;
};

SkyboxProgram gSkyboxProgram;

struct Scene{
  TriangleStripMesh plane_mesh;
  std::vector<GLuint> textures;
  std::vector<GLuint> textureUniforms;
  std::vector<glm::vec3> planes;

  // Model model;
};

struct Camera{
  glm::vec3 position;
  glm::vec3 look;
  glm::vec3 up;
  float yaw;
  float pitch;
  bool forwardKeyDown;
  bool backKeyDown;
  bool leftKeyDown;
  bool rightKeyDown;
};

void loadCubeMesh() {
  ///--- Generate a cube mesh for skybox, already done
  std::vector<glm::vec3> points;
  float w = 1;
  float h = 1;
  float d = 1;
  points.push_back(glm::vec3( w, h, d)); // 0
  points.push_back(glm::vec3(-w, h, d)); // 1
  points.push_back(glm::vec3( w, h,-d)); // 2
  points.push_back(glm::vec3(-w, h,-d)); // 3
  points.push_back(glm::vec3( w,-h, d)); // 4
  points.push_back(glm::vec3(-w,-h, d)); // 5
  points.push_back(glm::vec3(-w,-h,-d)); // 6
  points.push_back(glm::vec3( w,-h,-d)); // 7
  std::vector<unsigned int> indices = { 3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0 };

  GLfloat* vertexData = new GLfloat[points.size()*3];
  for(int i = 0; i < points.size(); i++){
    int OFFSET = 3;
    int loc = i*OFFSET;
    vertexData[loc] = points[i].x;
    vertexData[loc+1] = points[i].y;
    vertexData[loc+2] = points[i].z;
  }

  gSkyboxProgram.box_size = indices.size();

  //IBO data
  GLuint* indexData = new GLuint[indices.size()];
  for(int i = 0; i < indices.size(); i++){
    int loc = i;
    indexData[loc] = indices[i];
  }

  //Create VBO
  glGenBuffers(1, &gSkyboxProgram.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, gSkyboxProgram.VBO);
  glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

  //Create IBO
  glGenBuffers(1, &gSkyboxProgram.IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gSkyboxProgram.IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indexData, GL_STATIC_DRAW);

  delete[] vertexData;
  delete[] indexData;
}

int initScene(Scene& scene){
  glGenVertexArrays(1, &gVAO);
  glBindVertexArray(gVAO);

  gTerrainProgramID = loadShaderProgram(terrain_vshader, terrain_fshader);

  gVertAttrib = glGetAttribLocation(gTerrainProgramID, "vposition");
  if(gVertAttrib == -1){
    printf( "vposition is not a valid glsl program variable!\n" );
    return -1;
  }
  gUvCoordAttrib = glGetAttribLocation(gTerrainProgramID, "TexCoord");
  if(gUvCoordAttrib == -1){
    printf( "TexCoord is not a valid glsl program variable!\n" );
    return -1;
  }

  // Height texture
  scene.textureUniforms.push_back(glGetUniformLocation(gTerrainProgramID, "noiseTex"));
  scene.textures.push_back(loadAndBindNewTexture());
  noiseTexture();

  const std::string list[] = {"grass", "rock", "sand", "water", "snow"};
  for (int i=0 ; i < 5 ; ++i) {
    scene.textures.push_back(loadAndBindNewTexture());
    textureFromImage(("Textures/" + list[i]+ ".png").c_str());
    scene.textureUniforms.push_back(glGetUniformLocation(gTerrainProgramID, list[i].c_str()));
  }

  gCameraUniform = glGetUniformLocation(gTerrainProgramID, "viewPos");
  // checkerBoardTexture();

  // scene.model = createCubeModel();
  // loadModelToGL(scene.model);

  scene.plane_mesh = createPlane();
  loadTriangleStripIntoGL(scene.plane_mesh);

  // add grid of planes
  // scene.plane_textures.push_back(scene.textures[1]);
  // scene.planes.push_back(glm::vec3(0.0f,0.0f,0.0f));
  // scene.plane_textures.push_back(scene.textures[2]);
  scene.planes.push_back(glm::vec3(4.97f,0.0f,0.0f));

  gMatrixID = glGetUniformLocation(gTerrainProgramID, "MVP");

  // Skybox
  //
  //
  gSkyboxProgram.program_id = loadShaderProgram(skybox_vshader, skybox_fshader);
  gSkyboxProgram.vert_attrib = glGetAttribLocation(gSkyboxProgram.program_id, "vposition");
  if(gSkyboxProgram.vert_attrib == -1){
    printf( "vposition is not a valid glsl program variable!\n" );
    return -1;
  }

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_CUBE_MAP);

  glGenTextures(1, &gSkyboxProgram.texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyboxProgram.texture);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  textureCubeMapFromImages();

  gSkyboxProgram.view_id = glGetUniformLocation(gSkyboxProgram.program_id, "V");
  gSkyboxProgram.projection_id = glGetUniformLocation(gSkyboxProgram.program_id, "P");


  loadCubeMesh();

  return 0;
}

int renderScene(Scene& scene, Camera camera){
  glUseProgram(gTerrainProgramID);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 10000.0f);
  glm::mat4 view = glm::lookAt(
                               camera.position,
                               camera.position+camera.look, 
                               camera.up
                               );

  for(int j = 0; j < scene.textures.size(); j++){
    GLint texture = scene.textures[j];
    glActiveTexture(GL_TEXTURE0+j);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(scene.textureUniforms[j], j);
  }

  glUniform3fv(gCameraUniform, 1, &camera.position[0]);

  for(int i = 0; i < scene.planes.size(); i++){
    glm::vec3 plane_location = scene.planes[i];

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, plane_location);

    glm::mat4 mvp = projection * view * model;

    glEnableVertexAttribArray( gVertAttrib );

    RenderVars rv;
    rv.vertAttrib = gVertAttrib;
    rv.matrixID = gMatrixID;
    rv.otherAttrib = gUvCoordAttrib;

    // renderModel(scene.model, mvp, rv);
    renderTriangleStripMesh(scene.plane_mesh, mvp, rv);

  }

  glDisableVertexAttribArray( gVertAttrib );

  //
  // Skybox rendering
  //
  glUseProgram(gSkyboxProgram.program_id);


  glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyboxProgram.texture);

  glUniformMatrix4fv(gSkyboxProgram.view_id, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(gSkyboxProgram.projection_id, 1, GL_FALSE, &projection[0][0]);

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, gSkyboxProgram.VBO );
  glEnableVertexAttribArray( gSkyboxProgram.vert_attrib);
  glVertexAttribPointer( gSkyboxProgram.vert_attrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gSkyboxProgram.IBO );
  glDrawElements( GL_TRIANGLE_STRIP, gSkyboxProgram.box_size, GL_UNSIGNED_INT, NULL );

  glDisableVertexAttribArray( gSkyboxProgram.vert_attrib);
  //
  //

  //Disable vertex position

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
  Camera camera = {glm::vec3(10,110.5f,4), glm::vec3(0,0,0), glm::vec3(0,1,0), 0.0f, 0.0f};
  int PI = glm::pi<float>();
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
      if (event.type == SDL_KEYDOWN){
        switch(event.key.keysym.sym){
          case SDLK_q:
            done = true;
            break;
          case SDLK_w:
            camera.forwardKeyDown = true;
            break;
          case SDLK_s:
            camera.backKeyDown = true;
            break;
          case SDLK_a:
            camera.leftKeyDown = true;
            break;
          case SDLK_d:
            camera.rightKeyDown = true;
            break;
        }
      }
      if (event.type == SDL_KEYUP){
        switch(event.key.keysym.sym){
        case SDLK_w:
          camera.forwardKeyDown = false;
          break;
        case SDLK_s:
          camera.backKeyDown = false;
          break;
        case SDLK_a:
          camera.leftKeyDown = false;
          break;
        case SDLK_d:
          camera.rightKeyDown = false;
          break;
        }
      }
      if (event.type == SDL_MOUSEMOTION ){
        glm::vec2 delta = glm::vec2(event.motion.xrel, event.motion.yrel);
        delta.x = -delta.x;
        delta.y = -delta.y;
        float sensitivity = 0.005f;
        delta = sensitivity * delta;

        camera.yaw += delta[0];
        camera.pitch += delta[1];

        if(camera.pitch > PI/2.0f - 0.01f)  camera.pitch =  PI/2.0f - 0.01f;
        if(camera.pitch <  -PI/2.0f + 0.01f) camera.pitch =  -PI/2.0f + 0.01f;

        glm::vec3 front(0,0,0);
        front.x = sin(camera.yaw)*cos(camera.pitch);
        front.z = cos(camera.yaw)*cos(camera.pitch);
        front.y = sin(camera.pitch);

        camera.look = glm::normalize(front);
      }
    }

    // camera logic
    if(camera.forwardKeyDown || camera.backKeyDown || camera.leftKeyDown || camera.rightKeyDown){
      float movespeed = 0.3f;
      glm::vec3 movement = glm::vec3(0,0,0);
      if(camera.forwardKeyDown && !camera.backKeyDown){
        movement += camera.look;
      }
      if(camera.backKeyDown && !camera.forwardKeyDown){
        movement -= camera.look;
      }
      if(camera.leftKeyDown && !camera.rightKeyDown){
        movement -= glm::cross(camera.look, camera.up);
        // std::cout << "direction = " << movement.x << "," << movement.y << "," << movement.z << "\n";
      }
      if(camera.rightKeyDown && !camera.leftKeyDown){
        movement += glm::cross(camera.look, camera.up);
      }
      if(glm::length(movement) > 0.001f){
        movement = glm::normalize(movement);
        camera.position += movespeed * movement;
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
