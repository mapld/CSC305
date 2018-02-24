#include "imgui.h" // immediate mode gui
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h> // opengl wrapper
#include <SDL.h>
#include "gl_error_printing.h"

// global variables for now
GLuint gProgramID = 0;
GLint gVertexPos3DLocation = -1;
GLint texAttrib = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;
GLuint vaoId = 0;
GLuint tex;

int createScene(){
  glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    gProgramID = glCreateProgram();
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    const GLchar* vertexShaderSource[] = {
      "#version 140\n"
      "in vec3 LVertexPos3D;"
      "in vec2 TexCoord;"
      "out vec2 texcoord;"
      "void main() { gl_Position = vec4( LVertexPos3D.x, LVertexPos3D.y, LVertexPos3D.z, 1 ); texcoord=TexCoord; }"
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

    gVertexPos3DLocation = glGetAttribLocation(gProgramID, "LVertexPos3D");
    if(gVertexPos3DLocation == -1){
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // float pixels[] = {
    //   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    //   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    //   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    // };
    const int width = 24;
    const int height = 24;
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

      //VBO data
    GLfloat vertexData[] =
      {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f
      };

      //IBO data
    GLuint indexData[] = { 0, 1, 2,
                           0, 2, 3 };

    // glClearColor( 0.f, 0.f, 0.f, 1.f );

      //Create VBO
    glGenBuffers( 1, &gVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * 5 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

      //Create IBO
    glGenBuffers( 1, &gIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indexData, GL_STATIC_DRAW );

    return 0;
}

void renderScene(){
  glUseProgram(gProgramID);

  //Enable vertex position
  glEnableVertexAttribArray( gVertexPos3DLocation );

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, gVBO );
  glVertexAttribPointer( gVertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

  // texture coords
  glEnableVertexAttribArray( texAttrib);
  glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL );

  //Disable vertex position
  glDisableVertexAttribArray( gVertexPos3DLocation );

  //Unbind program
  glUseProgram( NULL );
}

int main(int, char**){
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
    std::cout << "Error: " << SDL_GetError() << "\n";
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
  SDL_Window *window = SDL_CreateWindow("CSC305 Assignment 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  gl3wInit();

  // ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplSdlGL3_Init(window);

  // ImGui style
  ImGui::StyleColorsDark();

  if(createScene() != 0){
    std::cout << "Something went wrong when creating scene";
  }

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
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClear(GL_COLOR_BUFFER_BIT);

    renderScene();

    ImGui::Render();
    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  return 0;
}
