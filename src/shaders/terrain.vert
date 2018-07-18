#version 430 core

in vec3 position;
in vec4 color;
in vec3 normal;

uniform mat4x4 model;
uniform mat4x4 view;
uniform mat4x4 projection;

out vec4 v_color;
out vec4 v_normal;

void main(void) {
    gl_Position = projection * view * model * vec4(position, 1.0);
    v_normal = model * vec4(normal, 1.0);
    v_color = color;
}
