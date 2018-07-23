#version 430 core

in vec3 v_position;
in vec4 v_normal;

out vec4 FragColor;

void main(void) {
    float radius = length(v_position);
    vec3 color = vec3(0.0, 0.0, 0.0);

    if (radius < 2.00) {
        color = vec3(0.8, 0.7, 0.4);
    } else if (radius < 2.08) {
        color = vec3(0.2, 0.6, 0.2);
    } else if (radius < 2.15) {
        color = vec3(0.5, 0.4, 0.3);
    } else {
        color = vec3(0.8, 0.8, 0.8);
    }
    FragColor = vec4(color.rgb * v_normal.z, 1.0);
}
