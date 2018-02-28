#pragma once
#include <iostream>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.

void printProgramLog( GLuint program ){
    int infoLogLength = 0;
    int maxLength = infoLogLength;

    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

    char* infoLog = new char[ maxLength ];

    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if(infoLogLength > 0){
      std::cout << infoLog << "\n";
    }
    delete[] infoLog;
}

void printShaderLog( GLuint shader){
  int infoLogLength = 0;
  int maxLength = infoLogLength;

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

  char* infoLog = new char[maxLength];

  glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog );
  if( infoLogLength > 0 ){
    std::cout << infoLog << "\n";
  }
  delete[] infoLog;
}
