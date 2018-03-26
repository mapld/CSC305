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
#include "animations.h"
#include "bezier.h"

int SCREEN_HEIGHT= 720;
int SCREEN_WIDTH=  1280;

GLuint gProgramID = 0;
GLint gVertAttrib = -1;
GLuint gVAO = -1;
GLuint matrixID;
GLint colorAttrib = -1;
GLuint tex;

int BAT_BODY_I = 0;
int BAT_LEFT_WING_I = 1;
int BAT_RIGHT_WING_I = 2;
int POINT_I = 3;

void loadLinesIntoGL(BezierCurveAnimation& anim){
  int NUM_VERTS = anim.linePoints.size();
  int NUM_LINES = NUM_VERTS;

  int OFFSET = 6;
  GLfloat* vertexData = new GLfloat[anim.linePoints.size()*OFFSET];
  for(int i = 0; i < anim.linePoints.size(); i++){
    int loc = i*OFFSET;
    glm::vec3 vert = anim.linePoints[i];
    vertexData[loc] = vert.x;
    vertexData[loc+1] = vert.y;
    vertexData[loc+2] = vert.z;
    vertexData[loc+3] = 0.9f;
    vertexData[loc+4] = 0.1f;
    vertexData[loc+5] = 0.1f;
  }

  GLint* indexData = new GLint[anim.linePoints.size()];
  for(int i = 0; i < anim.linePoints.size(); i++){
    indexData[i] = i;
  }

  //Create VBO
  if(!anim.lineVBO){
    glGenBuffers(1, &anim.lineVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, anim.lineVBO);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

  //Create IBO
  if(!anim.lineIBO){
    glGenBuffers(1, &anim.lineIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, anim.lineIBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_LINES * sizeof(GLuint), indexData, GL_STATIC_DRAW);
}

void loadModelIntoGL(Model& model){
  int NUM_TRIANGLES = model.numTriangles();
  int NUM_VERTS = model.numVerticies();

  int OFFSET = 6;
  GLfloat* vertexData = new GLfloat[model.vertices.size()*OFFSET];
  for(int i = 0; i < model.vertices.size(); i++){
    int loc = i*OFFSET;
    vertexData[loc] = model.vertices[i].x;
    vertexData[loc+1] = model.vertices[i].y;
    vertexData[loc+2] = model.vertices[i].z;
    vertexData[loc+3] = model.colors[i].x;
    vertexData[loc+4] = model.colors[i].y;
    vertexData[loc+5] = model.colors[i].z;
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
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTS * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    //Create IBO
  glGenBuffers(1, &model.IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRIANGLES * sizeof(GLuint), indexData, GL_STATIC_DRAW);


}

// TODO : provide the animation to this function (get rid of scaleDiff and bat path time)
void addBat(Scene& scene, std::vector<glm::vec3> curvePoints, float BAT_PATH_TIME = 3.5f, float scaleDiff = 1.0f){
    Model body = createBatBody();
    loadModelIntoGL(body);
    scene.models[BAT_BODY_I] = body;

    Model leftWing = createBatWing();
    loadModelIntoGL(leftWing);
    scene.models[BAT_LEFT_WING_I] = leftWing;

    Model rightWing = createBatWing();
    loadModelIntoGL(rightWing);
    scene.models[BAT_RIGHT_WING_I] = rightWing;

    // add models to scene:
    int body_inst_num = addModelToScene(scene, BAT_BODY_I);
    int left_wing_inst = addModelToScene(scene, BAT_LEFT_WING_I, body_inst_num, glm::vec3(-0.1f,0,0));
    int right_wing_inst = addModelToScene(scene, BAT_RIGHT_WING_I, body_inst_num, glm::vec3(0.1f,0,0));

    // add animations
    addLinearScaleAnimationToScene(scene, body_inst_num, BAT_PATH_TIME, 0.2f, 0.2f+scaleDiff);
    addWingAnimation(scene, left_wing_inst, true, 0.6f);
    addWingAnimation(scene, right_wing_inst, false, 0.6f);

    Model point = createRectModel(0.01f, 0.01f, RED);
    loadModelIntoGL(point);

    // Model secondModel = createRectModel(0.4,0.4);
    // loadModelIntoGL(secondModel);
    // scene.models.push_back(secondModel);

    matrixID = glGetUniformLocation(gProgramID, "MVP");

    BezierCurveAnimation anim;
    anim.animation_base.model_inst = body_inst_num;
    anim.animation_base.animation_total_time = BAT_PATH_TIME;
    anim.pointModel = point; // totally inconsistently copying model for each animation instead of using a reference (ModelInst) to it like elsewhere ¯\_(ツ)_/¯
    anim.points = curvePoints;

    addCurveFromPoints(anim);
    loadLinesIntoGL(anim);

    addBezierCurveAnimation(scene, anim);
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
      "in vec3 incolor;"
      "out vec3 v_color;"
      "uniform mat4 MVP;"
      "void main() {"
      "gl_Position = MVP * vec4( LVertexPos3D.x, LVertexPos3D.y, LVertexPos3D.z, 1 );"
      "v_color=incolor;"
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
        "in vec3 v_color;"
        "out vec4 LFragment;"
        "void main() { LFragment = vec4( v_color , 1.0 ); }"
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
    colorAttrib = glGetAttribLocation(gProgramID, "incolor");
    if(colorAttrib == -1){
      printf( "incolor is not a valid glsl program variable!\n" );
      return -1;
    }

    //texture
    // glGenTextures(1, &tex);
    // glBindTexture(GL_TEXTURE_2D, tex);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // if(textureFilename.size() > 0){
    //   textureFromImage(textureFilename.c_str());
    // }
    // checkerBoardTexture();
    // solidTexture();

    // add first bat to scene
    std::vector<glm::vec3> curve_points;
    curve_points.push_back(glm::vec3(0.1,0.2,0.5));
    curve_points.push_back(glm::vec3(0.7,0.2,0.5));
    curve_points.push_back(glm::vec3(0.5,0.5,0.5));
    curve_points.push_back(glm::vec3(0.6,0.8,0.5));
    addBat(scene, curve_points, 3.5f, 1.0f);

    // second bat
    curve_points.clear();
    curve_points.push_back(glm::vec3(1.6,0.2,0.5));
    curve_points.push_back(glm::vec3(0.5,0.2,0.5));
    curve_points.push_back(glm::vec3(0.3,0.5,0.5));
    curve_points.push_back(glm::vec3(0.3,0.1,0.5));
    addBat(scene, curve_points, 7.5f, 0.0f);

    return 0;
}



void renderLines(BezierCurveAnimation& animation){
    glm::mat4 mvp = animation.lineMVP;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

    //Set vertex data
    glBindBuffer( GL_ARRAY_BUFFER, animation.lineVBO );
    glVertexAttribPointer( gVertAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);

    // texture coords
    glEnableVertexAttribArray( colorAttrib);
    glVertexAttribPointer( colorAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    //Set index data and render
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, animation.lineIBO );
    glDrawElements( GL_LINE_STRIP, animation.linePoints.size(), GL_UNSIGNED_INT, NULL );
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
    for(int j = 0; j < scene.animations.bezier.size(); j++){
      BezierCurveAnimation anim = scene.animations.bezier[j];
      // std::cout << "mouse: " << mouseX << "\n";
      for(int i = 0; i < anim.points.size(); i++){
        glm::vec3 point = anim.points[i];
        // std::cout << "point:" << point.x << "\n";
        if(abs(mouseX - point.x) + abs(mouseY - point.y) < MOUSE_THRESHOLD){
          std::cout << "mouse: " << mouseX << "\n";
          scene.pointUnderMouse = i;
          scene.curveUnderMouse = j;
        }
      }
    }
  }
  else{
    if(scene.curveUnderMouse == -1){
      return;
    }
    BezierCurveAnimation& anim = scene.animations.bezier[scene.curveUnderMouse];
    anim.points[scene.pointUnderMouse].x = mouseX;
    anim.points[scene.pointUnderMouse].y = mouseY;
  }
}


void transformScene(glm::vec3 camera, Uint32 time, Scene& scene){
  if(scene.editMode){
    movePointUnderMouse(scene);
  }


  float secondsPassed = (float)time / 1000.0f;
  scene.mvps.clear();

  applyAnimations(scene, secondsPassed);

  glm::mat4 projection = glm::ortho(0.0f, (float) SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 0.0f, 1.0f , 100.0f);
  glm::mat4 view = glm::lookAt(
                               camera, // Camera is at (4,3,3), in World Space
                               glm::vec3(0,0,0), // looks at the origin
                               glm::vec3(0,1,0)
                               );

  for(int inst_num = 0; inst_num < scene.modelInstances.size(); inst_num++){
    glm::mat4 model = glm::mat4(1.f);
    int parent_inst_num = scene.modelInstances[inst_num].parent;
    glm::vec3 parent_transform = glm::vec3(0,0,0);
    glm::vec3 parent_scaling = glm::vec3(1,1,1);
    if(parent_inst_num >= 0){
      parent_transform = scene.modelInstances[parent_inst_num].transform.position;
      parent_scaling = scene.modelInstances[parent_inst_num].transform.scale;
    }
    // std::cout << "inst_num: " << inst_num << " , parent: " << parent_inst_num << " , parent y: " << parent_transform.y << "\n";
    glm::vec3 child_scaling = scene.modelInstances[inst_num].transform.scale;
    glm::vec3 child_transform = scene.modelInstances[inst_num].transform.position;
    glm::vec3 final_child_transform = parent_transform + glm::vec3(child_transform.x*parent_scaling.x,
                                                                  child_transform.y*parent_scaling.y,
                                                                  child_transform.z*parent_scaling.z);
    scene.modelInstances[inst_num].transform.position = final_child_transform;
    model = glm::translate(model, final_child_transform);
    model = glm::scale(model, parent_scaling);
    model = glm::scale(model, child_scaling);
    model = glm::rotate(model, scene.modelInstances[inst_num].transform.rotation, glm::vec3(0,0,1));
    glm::mat4 mvp = projection * view * model;
    scene.mvps.push_back(mvp);
  }


  for(int j = 0; j < scene.animations.bezier.size(); j++){
    BezierCurveAnimation& anim = scene.animations.bezier[j];
    // point movement:
    // anim.points[1].x = 0.4 + secondsPassed / 50;
    for(int i = 0; i < anim.points.size(); i++){
      glm::mat4 model = glm::translate(glm::mat4(1.f), anim.points[i]);
      glm::mat4 mvp = projection * view * model;
      scene.mvps.push_back(mvp);
    }

    glm::mat4 model = glm::mat4(1.f);
    anim.lineMVP = projection * view * model;
  }

}


void renderScene( Scene& scene){
  glUseProgram(gProgramID);

  //Enable vertex position
  glEnableVertexAttribArray( gVertAttrib );

  int i;
  for(i = 0; i < scene.modelInstances.size(); i++){
    int mI = scene.modelInstances[i].modelNum;
    renderModel(scene.models[mI], scene.mvps[i]);
  }

  for(BezierCurveAnimation& anim : scene.animations.bezier){
    addCurveFromPoints(anim);
    loadLinesIntoGL(anim);
    if(scene.editMode){
      // any remaining mvps are drawn as points
      int start = i;
      for(;i-start < anim.points.size(); i++){
        renderModel(anim.pointModel, scene.mvps[i]);
      }

      renderLines(anim);
    }
  }

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
      if (ImGui::Button("Edit")){
        scene.editMode = !scene.editMode;
      }
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
