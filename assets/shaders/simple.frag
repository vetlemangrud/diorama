#version 430 core

out vec4 color;

struct Splat {
  vec3 position;
  vec3 normal;
  float f_values[45]; // Coefficients for a Spherical Harmonics
                               // function defining color
  float opacity;
  float covariance[6];
};

struct ComputeOutput {
  vec3 transformedPosition;
  vec3 cov;
  vec3 conic;
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

  for (int i = 0; i < 10000;i++) {
    if (sqrdist(gl_FragCoord.xy, clipToScreen(computedSplats[i].transformedPosition)) < 10000){
      //vec3 cov = computedSplats[i].cov;
      
      
      //vec2 diff = gl_FragCoord.xy - clipToScreen(vec3(computedSplats[i].transformedPosition));

      //// From https://en.wikipedia.org/wiki/Gaussian_function
      //float f = splats[i].opacity * exp(-(cov.x*pow(diff.x,2)+2*cov.y*diff.x*diff.y+cov.z*pow(diff.y,2)));
      //float alpha = clamp(f, 0.0, 1.);
      //color += alpha * computedSplats[i].color;

      vec3 conic = computedSplats[i].conic;

      vec2 diff = clipToScreen(vec3(computedSplats[i].transformedPosition))- gl_FragCoord.xy;
      float power = -0.5 * (conic.x * diff.x * diff.x + conic.z * diff.y * diff.y) - conic.y * diff.x * diff.y;

      if (power > 0.) {
          continue;
      }

      // Eq. (2) from 3D Gaussian splatting paper.
      float alpha = min(.99f, splats[i].opacity * exp(power));
      color = mix(color, computedSplats[i].color, alpha);

      }
  }
}
