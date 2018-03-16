#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include <SDL_image.h>
#include "gl_error_printing.h"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "models_2D.h"
#include "textures.h"
#include "scene.h"

int SCREEN_HEIGHT= 720;
int SCREEN_WIDTH=  1280;

GLuint gProgramID = 0;
GLint gVertAttrib = -1;
GLuint gVAO = -1;
GLuint matrixID;
GLint texAttrib = -1;
GLuint tex;

int BAT_BODY_I = 0;
int BAT_LEFT_WING_I = 1;
int BAT_RIGHT_WING_I = 2;
int POINT_I = 3;

void loadLinesIntoGL(Scene& scene){
  int NUM_VERTS = scene.linePoints.size();
  int NUM_LINES = NUM_VERTS;

  int OFFSET = 5;
  GLfloat* vertexData = new GLfloat[scene.linePoints.size()*OFFSET];
  for(int i = 0; i < scene.linePoints.size(); i++){
    int loc = i*OFFSET;
    glm::vec3 vert = scene.linePoints[i];
    vertexData[loc] = vert.x;
    vertexData[loc+1] = vert.y;
    vertexData[loc+2] = vert.z;
    vertexData[loc+3] = 0.0f;
    vertexData[loc+4] = 0.0f;
  }

  GLint* indexData = new GLint[scene.linePoints.size()];
  for(int i = 0; i < scene.linePoints.size(); i++){
    indexData[i] = i;
  }

  //Create VBO
  if(!scene.lineVBO){
    glGenBuffers(1, &scene.lineVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, scene.lineVBO);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * 5 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

  //Create IBO
  if(!scene.lineIBO){
    glGenBuffers(1, &scene.lineIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene.lineIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_LINES * sizeof(GLuint), indexData, GL_STATIC_DRAW);
}

void loadModelIntoGL(Model& model){
  int NUM_TRIANGLES = model.numTriangles();
  int NUM_VERTS = model.numVerticies();

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
  glGenBuffers(1, &model.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * 5 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    //Create IBO
  glGenBuffers(1, &model.IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRIANGLES * sizeof(GLuint), indexData, GL_STATIC_DRAW);


}

glm::vec3 getBezierPoint(Scene& scene, float t){
  std::vector<glm::vec3> controlPoints = scene.points;
  return powf((1-t), 3)*controlPoints[0]
    + 3.0f * powf((1-t),2) * t * controlPoints[1]
    + 3.0f * (1 - t) * powf(t,2) * controlPoints[2]
    + powf(t,3) * controlPoints[3];
}

void addCurveFromPoints(Scene& scene){
  scene.linePoints.clear();
  float NUM_LINES = 100;
  for(float t = 0; t < 1; t+= 1.0f/NUM_LINES){
    glm::vec3 bezier_point = getBezierPoint(scene, t);
    scene.linePoints.push_back(bezier_point);
  }
}

int initScene(Scene& scene){
  // std::string textureFilename = "cat.jpg";
    scene.models = std::vector<Model>(BAT_RIGHT_WING_I+1);

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

    // if(textureFilename.size() > 0){
    //   textureFromImage(textureFilename.c_str());
    // }
    checkerBoardTexture();

    Model body = createBatBody();
    loadModelIntoGL(body);
    scene.models[BAT_BODY_I] = body;

    Model leftWing = createBatWing();
    loadModelIntoGL(leftWing);
    scene.models[BAT_LEFT_WING_I] = leftWing;

    Model rightWing = createBatWing();
    loadModelIntoGL(rightWing);
    scene.models[BAT_RIGHT_WING_I] =rightWing;

    Model point = createRectModel(0.01f, 0.01f);
    loadModelIntoGL(point);
    scene.pointModel = point;

    // Model secondModel = createRectModel(0.4,0.4);
    // loadModelIntoGL(secondModel);
    // scene.models.push_back(secondModel);

    matrixID = glGetUniformLocation(gProgramID, "MVP");

    scene.points.push_back(glm::vec3(0.1,0.1,0.5));
    scene.points.push_back(glm::vec3(0.7,0.2,0.5));
    scene.points.push_back(glm::vec3(0.5,0.5,0.5));
    scene.points.push_back(glm::vec3(0.6,0.8,0.5));

    addCurveFromPoints(scene);

    loadLinesIntoGL(scene);

    return 0;
}

void renderModel(Model& m, glm::mat4 mvp){
  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, m.VBO );
  glVertexAttribPointer( gVertAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

  // texture coords
  glEnableVertexAttribArray( texAttrib);
  glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m.IBO );
  glDrawElements( GL_TRIANGLES, m.numTriangles(), GL_UNSIGNED_INT, NULL );
}

void renderLines(Scene& scene){
  glm::mat4 mvp = scene.lineMVP;
  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, scene.lineVBO );
  glVertexAttribPointer( gVertAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

  // texture coords
  glEnableVertexAttribArray( texAttrib);
  glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GLfloat)));

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, scene.lineIBO );
  glDrawElements( GL_LINE_STRIP, scene.linePoints.size(), GL_UNSIGNED_INT, NULL );
}

void movePointUnderMouse(Scene& scene){
  float MOUSE_THRESHOLD = 0.02;
  if(!scene.actionDown){
    return;
  }
  int mx,my;
  SDL_GetMouseState(&mx,&my);
  float mouseX = (float)mx / (float)SCREEN_HEIGHT;
  float mouseY = (float)my / (float)SCREEN_HEIGHT;
  if(scene.pointUnderMouse == -1){
    for(int i = 0; i < scene.points.size(); i++){
      glm::vec3 point = scene.points[i];
      if(abs(mouseX - point.x) + abs(mouseY - point.y) < MOUSE_THRESHOLD){
        std::cout << mouseX << "\n";
        scene.pointUnderMouse = i;
      }
    }
  }
  else{
    scene.points[scene.pointUnderMouse].x = mouseX;
    scene.points[scene.pointUnderMouse].y = mouseY;
  }
}

void transformScene(glm::vec3 camera, Uint32 time, Scene& scene){
  movePointUnderMouse(scene);

  float secondsPassed = (float)time / 1000.0f;
  scene.mvps.clear();

  int pi = glm::pi<float>();
  float topAngle = 0.5f;
  float bottomAngle = -0.5f;
  float totalAnimationTime = 1.5f;
  float wingAdjFactor = 0.05f;
  float curAnimationTime = fmod(secondsPassed, totalAnimationTime);

  float leftAngle;
  float wingAdj;
  if(curAnimationTime >= totalAnimationTime / 2.0f){
    float animationTimeAdj = (curAnimationTime - totalAnimationTime/2.0f) / (totalAnimationTime/2.0f);
    leftAngle = bottomAngle + animationTimeAdj*(topAngle - bottomAngle);
    wingAdj = wingAdjFactor - 2*(animationTimeAdj* wingAdjFactor);
  }
  else{
    float animationTimeAdj = curAnimationTime / (totalAnimationTime/2.0f);
    leftAngle = topAngle + animationTimeAdj*(bottomAngle - topAngle);
    wingAdj = -1*wingAdjFactor + 2*(animationTimeAdj* wingAdjFactor);
  }
  // std::cout << "time: " << curAnimationTime << "angle: " << leftAngle << "\n";

  float bezierAnimationTime = 2.0f;
  float cT = fmod(secondsPassed, bezierAnimationTime) / bezierAnimationTime;

  float ADJ_X_FOR_SCALE = 0.04f;

  // glm::vec3 bat_location =glm::vec3(1.f, 0.3f, 0.f);
  glm::vec3 bat_location = getBezierPoint(scene, cT);

  float scaleFactor = 0.2f + cT;
  glm::vec3 bat_scaling = glm::vec3(scaleFactor, scaleFactor, 1.f);

  wingAdj *= scaleFactor;


  // glm::mat4 projection = glm::ortho(0.0f, (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 0.0f, 0.1f, 100.0f);
  glm::mat4 projection = glm::ortho(0.0f, (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 0.0f, 1.0f , 100.0f);

  glm::mat4 view = glm::lookAt(
                               camera, // Camera is at (4,3,3), in World Space
                               glm::vec3(0,0,0), // looks at the origin
                               glm::vec3(0,1,0)
                               );

  glm::mat4 body_model = glm::translate(glm::mat4(1.f), bat_location);
  body_model = glm::scale(body_model, bat_scaling);

  glm::mat4 mvp = projection * view * body_model;
  scene.mvps.push_back(mvp);

  // left wing
  glm::mat4 model = glm::mat4(1.f);
  model = glm::translate(model, bat_location);
  model = glm::translate(model, glm::vec3(-0.1f *scaleFactor, wingAdj, 0.f));
  // model = glm::translate(model, glm::vec3(ADJ_X_FOR_SCALE/scaleFactor, 0, 0.f));
  model = glm::scale(model, bat_scaling);
  model = glm::rotate(model, leftAngle, glm::vec3(0,0,1));
  mvp = projection * view * model;
  scene.mvps.push_back(mvp);

  // right wing
  model = glm::mat4(1.f);
  model = glm::translate(model, bat_location);
  model = glm::translate(model, glm::vec3(0.1f *scaleFactor, wingAdj, 0.f));
  // model = glm::translate(model, glm::vec3(-ADJ_X_FOR_SCALE/scaleFactor, 0, 0.f));
  model = glm::scale(model, bat_scaling);
  model = glm::rotate(model, -leftAngle, glm::vec3(0,0,1));
  mvp = projection * view * model;
  scene.mvps.push_back(mvp);

  // points
  // point movement:
  // scene.points[1].x = 0.4 + secondsPassed / 50;

  for(int i = 0; i < scene.points.size(); i++){
    model = glm::translate(glm::mat4(1.f), scene.points[i]);
    mvp = projection * view * model;
    scene.mvps.push_back(mvp);
  }

  model = glm::mat4(1.f);
  scene.lineMVP = projection * view * model;
}


void renderScene( Scene& scene){
  glUseProgram(gProgramID);

  //Enable vertex position
  glEnableVertexAttribArray( gVertAttrib );

  int i;
  for(i = 0; i < scene.models.size(); i++){
    renderModel(scene.models[i], scene.mvps[i]);
  }

  addCurveFromPoints(scene);
  loadLinesIntoGL(scene);

  // any remaining mvps are drawn as points
  for(;i < scene.mvps.size(); i++){
    renderModel(scene.pointModel, scene.mvps[i]);
  }

  renderLines(scene);

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

  Scene scene;

  if(initScene(scene) != 0){
    std::cout << "Something went wrong when creating scene";
  }

  glm::vec3 camera = glm::vec3(0,0,4);

  Uint32 timePerFrame = 1000 / 60;

  Uint32 startTime = SDL_GetTicks();
  Uint32 prevTime = startTime;
  bool done = false;
  while (!done){
    Uint32 curTime = SDL_GetTicks();
    Uint32 deltaTime = curTime - prevTime;
    if(deltaTime < timePerFrame){
      continue;
    }
    prevTime = curTime;
    Uint32 relativeTime = curTime - startTime;
    SDL_Event event;
    while (SDL_PollEvent(&event)){
      ImGui_ImplSdlGL3_ProcessEvent(&event);
      if (event.type == SDL_QUIT){
        done = true;
      }
      if (event.type == SDL_MOUSEBUTTONDOWN){
        scene.actionDown = true;
      }
      if (event.type == SDL_MOUSEBUTTONUP){
        scene.actionDown = false;
        scene.pointUnderMouse = -1;
      }
    }
    ImGui_ImplSdlGL3_NewFrame(window);
    {
      // imgui stuff here
    }


    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0,0.5f,0.8f,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    transformScene(camera, relativeTime, scene);
    renderScene(scene);

    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  return 0;
}
