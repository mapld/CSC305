#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include <SDL_image.h>
#include "gl_error_printing.h"
#include "models.h"
#include "textures.h"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

int SCREEN_HEIGHT= 720;
int SCREEN_WIDTH=  1280;

// global variables for now
GLuint gProgramID = 0;
GLint gVertAttrib = -1;
GLuint matrixID;
GLint texAttrib = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;
GLuint gVAO = 0;
GLuint tex;

int NUM_TRIANGLES = 0;

int createScene(Model model, std::string textureFilename){
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    gProgramID = glCreateProgram();

    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    const GLchar* vertexShaderSource[] = {
      "#version 140\n"
      "in vec3 LVertexPos3D;"
      "in vec2 TexCoord;"
      "out vec2 texcoord;"
      "uniform mat4 MVP;"
      "void main() {"
      "gl_Position = MVP * vec4( LVertexPos3D.x, LVertexPos3D.y, LVertexPos3D.z, 1 );"
      "texcoord=TexCoord;"
      "}"
    };
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
    if( vShaderCompiled != GL_TRUE ){
        printf( "Unable to compile vertex shader %d!\n", vertexShader );
        return -1;
    }


    glAttachShader(gProgramID, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSource[] =
      {
        "#version 140\n"
        "in vec2 texcoord;"
        "out vec4 LFragment;"
        "uniform sampler2D tex;"
        "void main() { LFragment = texture(tex, texcoord) * vec4( 1.0, 1.0, 1.0, 1.0 ) + vec4( 0.1, 0.1, 0.1, 1.0 ); }"
      };
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);

    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
    if( fShaderCompiled != GL_TRUE ){
        printf( "Unable to compile fragment shader %d!\n", fragmentShader );
        printShaderLog( fragmentShader );
        return -1;
    }
    glAttachShader(gProgramID, fragmentShader);

    glLinkProgram(gProgramID);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
    if(programSuccess != GL_TRUE){
      printf( "Error linking program %d!\n", gProgramID );
      return -1;
    }

    gVertAttrib = glGetAttribLocation(gProgramID, "LVertexPos3D");
    if(gVertAttrib == -1){
      printf( "LVertexPos3D is not a valid glsl program variable!\n" );
      return -1;
    }
    texAttrib = glGetAttribLocation(gProgramID, "TexCoord");
    if(texAttrib == -1){
      printf( "TexCoord is not a valid glsl program variable!\n" );
      return -1;
    }

    //texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(textureFilename.size() > 0){
      textureFromImage(textureFilename.c_str());
    }
    else{
      checkerBoardTexture();
    }

   NUM_TRIANGLES = model.triangleIndicies.size()*3;
    int NUM_VERTS = model.vertices.size();

    GLfloat* vertexData = new GLfloat[model.vertices.size()*5];
    for(int i = 0; i < model.vertices.size(); i++){
      int OFFSET = 5;
      int loc = i*OFFSET;
      vertexData[loc] = model.vertices[i].x;
      vertexData[loc+1] = model.vertices[i].y;
      vertexData[loc+2] = model.vertices[i].z;
      vertexData[loc+3] = model.uvCoords[i].x;
      vertexData[loc+4] = model.uvCoords[i].y;
    }

     //IBO data
    GLint* indexData = new GLint[model.triangleIndicies.size()*3];
    for(int i = 0; i < model.triangleIndicies.size(); i++){
      int loc = i * 3;
      indexData[loc] = model.triangleIndicies[i].x;
      indexData[loc+1] = model.triangleIndicies[i].y;
      indexData[loc+2] = model.triangleIndicies[i].z;
    }

      //Create VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * 5 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

      //Create IBO
    glGenBuffers(1, &gIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRIANGLES * sizeof(GLuint), indexData, GL_STATIC_DRAW);

    matrixID = glGetUniformLocation(gProgramID, "MVP");

    return 0;
}

void renderScene(glm::vec3 camera){
  glUseProgram(gProgramID);

  // mvp 
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

  glm::mat4 view = glm::lookAt(
                               camera, // Camera is at (4,3,3), in World Space
                               glm::vec3(0,0,0), // looks at the origin
                               glm::vec3(0,1,0)
                               );

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 mvp = projection * view * model;


  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

  //Enable vertex position
  glEnableVertexAttribArray( gVertAttrib );

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, gVBO );
  glVertexAttribPointer( gVertAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

  // texture coords
  glEnableVertexAttribArray( texAttrib);
  glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
  glDrawElements( GL_TRIANGLES, NUM_TRIANGLES, GL_UNSIGNED_INT, NULL );

  //Disable vertex position
  glDisableVertexAttribArray( gVertAttrib );

  //Unbind program
  glUseProgram( NULL );
}

int main(int, char**){
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
    std::cout << "Error: " << SDL_GetError() << "\n";
    return -1;
  }

  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if(!(IMG_Init(imgFlags) & imgFlags)){
    std::cout << "Image Error: " << SDL_GetError() << "\n";
    return -1;
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
  SDL_Window *window = SDL_CreateWindow("CSC305 Assignment 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  gl3wInit();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplSdlGL3_Init(window);

  // ImGui style
  ImGui::StyleColorsDark();

  // Model model = createCubeModel();
  // Model model = createSphereModel(glm::vec3(0,0,0), 1.0f, 200, 200);
  // Model model = createCylinderModel(glm::vec3(0,0,0), 1.0f, 2.0f, 200);
  Model model = createCubeModel();

  // writeModelToObj(model, "cylinder.obj");

  if(createScene(model, "") != 0){
    std::cout << "Something went wrong when creating scene";
  }

  glm::vec3 camera = glm::vec3(4,3,4);

  bool done = false;
  while (!done){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
      ImGui_ImplSdlGL3_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
    }
    ImGui_ImplSdlGL3_NewFrame(window);

    {
      static char modelName[1024] = "";
      static char textureName[1024] = "";
      if (ImGui::Button("Cylinder")){
        model = createCylinderModel(glm::vec3(0,0,0), 1.0f, 2.0f, 200);
        createScene(model, textureName);
      }
      if (ImGui::Button("Sphere")){
        model = createSphereModel(glm::vec3(0,0,0), 1.0f, 200, 200);
        createScene(model, textureName);
      }
      if (ImGui::Button("Cube")){
        model = createCubeModel();
        createScene(model, textureName);
      }

      // ImGui::SameLine();
      ImGui::InputText("model", modelName, sizeof(modelName));
      ImGui::InputText("texture", textureName, sizeof(textureName));
      if (ImGui::Button("Load")){
        if(strlen(modelName) != 0){
          loadModelFromObj(modelName, model);
        }
        createScene(model, textureName);
      }
      if (ImGui::Button("Save")){
        if(strlen(modelName) != 0){
          writeModelToObj(model, modelName);
        }
      }

      ImGui::SliderFloat("camera x", &camera.x, -5.0f, 5.0f);
      ImGui::SliderFloat("camera y", &camera.y, -5.0f, 5.0f);
      ImGui::SliderFloat("camera z", &camera.z, -5.0f, 5.0f);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene(camera);

    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  return 0;
}
