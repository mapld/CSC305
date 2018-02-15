#version 330 core
// When you edit these shaders, Clear CMake Configuration so they are copied to build folders
in vec2 uv;

out vec4 FragColor;

uniform int hasNormals;
uniform int hasTextures;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex,uv).rgba;
}
