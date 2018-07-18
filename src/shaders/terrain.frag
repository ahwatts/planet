#version 430 core

in vec4 v_color;
in vec4 v_normal;

out vec4 FragColor;

void main(void) {
    FragColor = vec4(v_color.rgb * v_normal.z, 1.0);
}
