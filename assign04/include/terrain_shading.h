#pragma once
#include <GL/gl3w.h> // opengl wrapper

int loadTerrainShaders(){
  GLuint program_id = glCreateProgram();

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


    glAttachShader(program_id, vertexShader);

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
