#pragma once
#include <GL/gl3w.h> // opengl wrapper


const GLchar* skybox_vshader =
#include "skybox_vshader.glsl"
  ;
const GLchar* skybox_fshader =
#include "skybox_fshader.glsl"
  ;

const GLchar* terrain_vshader =
#include "terrain_vshader.glsl"
  ;
const GLchar* terrain_fshader =
#include "terrain_fshader.glsl"
  ;


int loadShaderProgram(const GLchar* vshader, const GLchar* fshader){
  GLuint program_id = glCreateProgram();

  GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    const GLchar* vertexShaderSource[] = {vshader};
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vShaderCompiled );
    if( vShaderCompiled != GL_TRUE ){
        printf( "Unable to compile vertex shader %d!\n", vertexShader );
        return -1;
    }


    glAttachShader(program_id, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSource[] = {fshader};
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);

    GLint fShaderCompiled = GL_FALSE;
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
    if( fShaderCompiled != GL_TRUE ){
        printf( "Unable to compile fragment shader %d!\n", fragmentShader );
        printShaderLog( fragmentShader );
        return -1;
    }
    glAttachShader(program_id, fragmentShader);

    glLinkProgram(program_id);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &programSuccess);
    if(programSuccess != GL_TRUE){
      printf( "Error linking program %d!\n", program_id);
      return -1;
    }

    return program_id;
}
