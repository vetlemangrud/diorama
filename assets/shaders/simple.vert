#version 430 core

layout (location = 0) in vec3 aPos;

layout (std430, binding = 0) buffer Positions {
    vec4 positions[];
};
layout (std430, binding = 1) buffer Scales {
    vec4 scales[];
};
struct FValues {
    float values[48];
};
layout (std430, binding = 2) buffer Colors {
    FValues colors[];
};
layout (std430, binding = 3) buffer Rotations {
    vec4 rotations[];
};
layout (std430, binding = 4) buffer Opacities {
    float opacities[];
};


out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float[9] SHNewEval3(const float fX, const float fY, const float fZ) {
  float fC0, fC1, fS0, fS1, fTmpA, fTmpB, fTmpC;
  float fZ2 = fZ * fZ;
  float pSH[9];
  pSH[0] = 0.2820947917738781f;
  pSH[2] = 0.4886025119029199f * fZ;
  pSH[6] = 0.9461746957575601f * fZ2 + -0.3153915652525201f;
  fC0 = fX;
  fS0 = fY;
  fTmpA = -0.48860251190292f;
  pSH[3] = fTmpA * fC0;
  pSH[1] = fTmpA * fS0;
  fTmpB = -1.092548430592079f * fZ;
  pSH[7] = fTmpB * fC0;
  pSH[5] = fTmpB * fS0;
  fC1 = fX*fC0 - fY*fS0;
  fS1 = fX*fS0 + fY*fC0;
  fTmpC = 0.5462742152960395f;
  pSH[8] = fTmpC * fC1;
  pSH[4] = fTmpC * fS1;
  return pSH;
}

mat3 quatToMat(vec4 q) {
    return mat3(2.0 * (q.x * q.x + q.y * q.y) - 1.0, 2.0 * (q.y * q.z + q.x * q.w), 2.0 * (q.y * q.w - q.x * q.z), // 1st column
                2.0 * (q.y * q.z - q.x * q.w), 2.0 * (q.x * q.x + q.z * q.z) - 1.0, 2.0 * (q.z * q.w + q.x * q.y), // 2nd column
                2.0 * (q.y * q.w + q.x * q.z), 2.0 * (q.z * q.w - q.x * q.y), 2.0 * (q.x * q.x + q.w * q.w) - 1.0); // last column
}

void main() {
    
    vec3 scale = vec3(scales[gl_InstanceID]);
    mat3 rotation = quatToMat(rotations[gl_InstanceID]);
    vec3 vertexPos = rotation * (scale * aPos) + vec3(positions[gl_InstanceID]);

    gl_Position = projection * view * model * vec4(vertexPos, 1.0);

    // Color
    float f_values[48] = colors[gl_InstanceID].values;
    vec3 direction = normalize(vec3(positions[gl_InstanceID]) -vec3(0,1,5));
    float sPH[9] = SHNewEval3(direction.x, direction.y, direction.z);
    vec3 c = 0.5 + sPH[0] * vec3(f_values[0], f_values[1], f_values[2]);
    for(int i = 1; i < 9; i++) {
      float redC = f_values[3+i-1];
      float greenC = f_values[3+15+i-1];
      float blueC = f_values[3+2*15+i-1]; 
      c += sPH[i] * vec3(redC, greenC, blueC); 
    }

    color = vec3(colors[gl_InstanceID].values[0], colors[gl_InstanceID].values[1], colors[gl_InstanceID].values[2]) * 0.2 + vec3(0.5);
    //color = c;
}

