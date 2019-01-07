#version 430 core

const int MAX_LIGHTS = 10;
struct LightInfo {
    bool enabled;
    vec3 direction;
    // vec4 color;
    // uint specular_exp;
};

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;

layout(shared) uniform LightListBlock {
    LightInfo lights[MAX_LIGHTS];
};

// uniform uint specular_exp;

layout(location = 0) out vec4 outColor;

void main(void) {
    int enabled_lights = 0;
    vec3 diffuse_colors[MAX_LIGHTS];
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights[i].enabled) {
            enabled_lights += 1;
            diffuse_colors[i] = inColor.rgb * dot(inNormal, -1 * lights[i].direction);
        }
    }

    vec3 ambient_color = inColor.rgb;

    vec3 diffuse_color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (lights[i].enabled) {
            diffuse_color += (1.0 / enabled_lights) * diffuse_colors[i];
        }
    }

    outColor = vec4(0.1*ambient_color + 0.9*diffuse_color, 1.0);
}
