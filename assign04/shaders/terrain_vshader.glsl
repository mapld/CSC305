R"(
#version 330 core
uniform sampler2D noiseTex;

in vec3 vposition;
in vec2 TexCoord;

uniform mat4 MVP;

out vec2 uv;
out vec3 fragPos;
out float heightFactor;

void main() {
    uv = TexCoord;

    float hf = 20.0f;

    /// TODO: Get height h at uv
    float h = (texture(noiseTex,uv).r + 1.0f) / 2.0f;

    if(h < 0.35f) h = 0.35f;

    h *= hf;

    fragPos = vposition.xyz + vec3(0,h,0);
    gl_Position = MVP*vec4(vposition.x, vposition.y + h, vposition.z, 1.0);
    heightFactor = hf;
}
)"
