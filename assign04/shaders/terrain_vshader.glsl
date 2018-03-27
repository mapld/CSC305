R"(
#version 330 core
uniform sampler2D noiseTex;

in vec3 vposition;
in vec2 TexCoord;

uniform mat4 MVP;

out vec2 uv;
out vec3 fragPos;

void main() {
    /// TODO: Get height h at uv
    float h = 0.0;

    uv = TexCoord;
    fragPos = vposition.xyz + vec3(0,0,h);
    gl_Position = MVP*vec4(vposition.x, vposition.y, vposition.z + h, 1.0);
}
)"
