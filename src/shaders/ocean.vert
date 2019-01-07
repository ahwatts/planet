#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(shared) uniform ViewAndProjectionBlock {
    mat4x4 view;
    mat4x4 view_inv;
    mat4x4 projection;
};

uniform mat4x4 model;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

void main(void) {
    gl_Position = projection * view * model * vec4(inPosition, 1.0);
    outNormal = mat3x3(model) * inNormal;
    outColor = inColor;
}
