#version 430 core

const int MAX_LIGHTS = 10;
struct LightInfo {
    bool enabled;
    vec3 position;
    // vec4 color;
    // uint specular_exp;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(shared) uniform ViewAndProjectionBlock {
    mat4x4 view;
    mat4x4 view_inv;
    mat4x4 projection;
};

layout(shared) uniform LightListBlock {
    LightInfo lights[MAX_LIGHTS];
};

uniform mat4x4 model;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;

void main(void) {
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    outNormal = model * vec4(inNormal, 1.0);
    outColor = inColor;
}
