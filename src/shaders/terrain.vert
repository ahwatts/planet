#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(shared) uniform ViewAndProjectionBlock {
    mat4x4 view;
    mat4x4 projection;
};

uniform mat4x4 model;

out vec3 v_position;
out vec4 v_normal;

void main(void) {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_position = position;
    v_normal = model * vec4(normal, 1.0);
}
