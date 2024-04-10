#version 430 core

out vec4 color;

struct Splat {
  vec3 position;
  vec3 normal;
  float f_values[45]; // Coefficients for a Spherical Harmonics
                               // function defining color
  float opacity;
  vec3 scale;
  vec4 rotation;
};

struct ComputeOutput {
  vec3 transformedPosition;
  vec4 color;
};

// SSBO to store original splat data
layout(std430, binding = 0) buffer Splats {
  Splat splats[];
};

// SSBO to store transformed splat positions
layout(std430, binding = 1) buffer TransformedSplats {
  ComputeOutput computedSplats[];
};

uniform int width;
uniform int height;

float sqrdist(vec2 a, vec2 b) {
  return pow(a.x-b.x,2) + pow(a.y-b.y,2); 
};

vec2 clipToScreen(vec3 clipPos){
  vec2 pos = vec2(0.0);
  pos.x = (clipPos.x + 2) * width/5;
  pos.y = (clipPos.y + 2) * height/5;
  return pos;
};
float sig(float x) {

    return 1.0 / (1.0 + exp(-x)); 
}

void main()
{
  color = vec4(0.0);

  for (int i = 0; i < splats.length(); i++) {
    if(sqrdist(vec2(gl_FragCoord), clipToScreen(vec3(splats[i].position))) < 16) {
      color = computedSplats[i].color;
    }
  }
}
