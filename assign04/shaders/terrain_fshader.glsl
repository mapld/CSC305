R"(
#version 330 core
uniform sampler2D noiseTex;

uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D sand;
uniform sampler2D snow;
uniform sampler2D water;

// The camera position
uniform vec3 viewPos;

in vec2 uv;
in float heightFactor;
in float waterHeight;
// Fragment position in world space coordinates
in vec3 fragPos;

out vec4 color;

void main() {

    float f_width = 100.0f;
    float f_height = 100.0f;

    // Directional light source
    // vec3 lightDir = normalize(vec3(1,1,1));
    vec3 lightSource = vec3(10,50,5);

    // Texture size in pixels
    ivec2 size = textureSize(noiseTex, 0);

    const int slopeOffset = 3;

    /// TODO: Calculate surface normal N
    /// HINT: Use textureOffset(,,) to read height at uv + pixelwise offset
    /// HINT: Account for texture x,y dimensions in world space coordinates (default f_width=f_height=5)
    vec3 A = vec3( fragPos.x + slopeOffset*(f_width / size.x), heightFactor * textureOffset(noiseTex, uv, ivec2(slopeOffset,0)).r, fragPos.z );
    vec3 B = vec3( fragPos.x - slopeOffset*(f_width / size.x), heightFactor * textureOffset(noiseTex, uv, ivec2(-slopeOffset,0)).r, fragPos.z );
    vec3 C = vec3( fragPos.x, heightFactor * (textureOffset(noiseTex, uv, ivec2(0,slopeOffset)).r + 1.0f) / 2.0f , fragPos.z + slopeOffset*(f_height / size.y));
    vec3 D = vec3( fragPos.x, heightFactor * (textureOffset(noiseTex, uv, ivec2(0,-slopeOffset)).r + 1.0f) / 2.0f, fragPos.z - slopeOffset*(f_height / size.y));
    vec3 N = normalize( cross( normalize(C-D), normalize(A-B)) );

    float h = (texture(noiseTex,uv).r + 1.0f) / 2.0f;
    if(h < waterHeight){
      N = normalize(vec3(0,1,0));
    }

    /// TODO: Texture according to height and slope
    /// HINT: Read noiseTex for height at uv
    float slope = 0.001f;
    slope = max(slope, (A.y - fragPos.y) / (slopeOffset*f_width / size.x));
    slope = max(slope, (B.y - fragPos.y) / (slopeOffset*f_width / size.x));
    slope = max(slope, (C.y - fragPos.y) / (slopeOffset*f_width / size.x));
    slope = max(slope, (D.y - fragPos.y) / (slopeOffset*f_width / size.x));

    // base intensity
    float intensity = 0.4f;

    // diffuse
    float diffuseFactor = 0.5f;
    vec3 L = normalize(lightSource - fragPos);
    intensity += diffuseFactor * (max(0,dot(N,L)));

    // specular
    float specularFactor = 0.3f;
    float phongExponent = 30.0f;
    vec3 V = viewPos - fragPos;
    vec3 halfway = normalize(L + V);
    intensity += specularFactor* max(0, pow(dot(N,halfway), phongExponent));
    /// HINT: max(,) dot(,) reflect(,) normalize();

    float slopeSnowThreshold = 0.3f;
    float slopeGrassThreshold = 0.6f;
    float lowGrassThreshold = 1.75f;

    vec4 c = texture(grass,uv);
    if(slope > lowGrassThreshold){
      c = texture(rock,uv);
    }
    if(h > 0.50f){
      c = texture(rock,uv);
      if(slope < slopeGrassThreshold){
        c = texture(grass,uv);
      }
      // if(slope < 0.1f){
      //   c = texture(snow,uv);
      // }
    }
    if(h > 0.65f){
      c = texture(rock,uv);
      if(slope < slopeSnowThreshold){
        c = texture(snow,uv);
      }
    }
    if(h < 0.351f){
      c = texture(sand,uv);
    }
    if(h < waterHeight){
      c = texture(water,uv);
    }

    float thing = A.y - fragPos.y;
    // if(viewPos.x > 25.0f) c = vec4(1,0,0,0);
    // c = vec4(1,0,0,0);

    color = intensity * vec4(c);
}
)"
