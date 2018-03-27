#pragma once
#include <GL/gl3w.h> // opengl wrapper

#include <vector>
#include <glm/common.hpp>

const unsigned resPrim = 999999; // The index at which we begin a new triangle strip

struct TriangleStripMesh{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uv_coords;
  std::vector<int> indices;

  GLuint VBO;
  GLuint IBO;
};

TriangleStripMesh createPlane(glm::vec2 size = glm::vec2(5.0f, 5.0f)){
  TriangleStripMesh mesh;

  int n_width = 256; // Grid resolution
  int n_height = 256;
  float f_width = size.x; // Grid width, centered at 0,0
  float f_height = size.y;

  ///--- Vertex positions, tex coords
  for(int j=0; j<n_height; ++j) {
    for(int i=0; i<n_width; ++i) {
      float x = (-f_width/2) + ((float)j / (float)n_width)*f_width;
      float y = (-f_height/2) + ((float)i / (float)n_height)*f_height;
      mesh.vertices.push_back(glm::vec3(x, 0.0f, y));
      mesh.uv_coords.push_back( glm::vec2( i/(float)(n_width-1), j/(float)(n_height-1)) );
    }
  }

  ///--- Element indices using triangle strips
  for(int j=0; j<n_height-1; ++j) {
    ///--- The two vertices at the base of each strip
    mesh.indices.push_back(j*n_width);
    mesh.indices.push_back((j+1)*n_width);
    for(int i=1; i<n_width; ++i) {
      mesh.indices.push_back(j*n_width + i);
      mesh.indices.push_back((j+1)*n_width + i);
    }
    ///--- A new strip will begin when this index is reached
    mesh.indices.push_back(resPrim);
  }

  return mesh;
}


void loadTriangleStripIntoGL(TriangleStripMesh& mesh){
  GLfloat* vertexData = new GLfloat[mesh.vertices.size()*5];
  for(int i = 0; i < mesh.vertices.size(); i++){
    int OFFSET = 5;
    int loc = i*OFFSET;
    vertexData[loc] = mesh.vertices[i].x;
    vertexData[loc+1] = mesh.vertices[i].y;
    vertexData[loc+2] = mesh.vertices[i].z;
    vertexData[loc+3] = mesh.uv_coords[i].x;
    vertexData[loc+4] = mesh.uv_coords[i].y;

  }

  //IBO data
  GLint* indexData = new GLint[mesh.indices.size()*3];
  for(int i = 0; i < mesh.indices.size(); i++){
    int loc = i;
    indexData[loc] = mesh.indices[i];
  }

  //Create VBO
  glGenBuffers(1, &mesh.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * 5 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

  //Create IBO
  glGenBuffers(1, &mesh.IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint), indexData, GL_STATIC_DRAW);

  delete[] vertexData;
  delete[] indexData;
}

void renderTriangleStripMesh(TriangleStripMesh& m, glm::mat4 mvp, RenderVars rv, int offset = 2){
  glUniformMatrix4fv(rv.matrixID, 1, GL_FALSE, &mvp[0][0]);

  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(resPrim);

  //Set vertex data
  glBindBuffer( GL_ARRAY_BUFFER, m.VBO );
  glVertexAttribPointer( rv.vertAttrib, 3, GL_FLOAT, GL_FALSE, (offset+3) * sizeof(GLfloat), NULL);

  // texture coords
  glEnableVertexAttribArray( rv.otherAttrib);
  glVertexAttribPointer( rv.otherAttrib, offset, GL_FLOAT, GL_FALSE, (offset+3) * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

  //Set index data and render
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m.IBO );
  glDrawElements( GL_TRIANGLE_STRIP, m.indices.size(), GL_UNSIGNED_INT, NULL );
}
