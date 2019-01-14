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
layout(location = 2) out vec3 outEyeDir;

void main(void) {
    vec4 wld_vert_pos4 = view * model * vec4(inPosition, 1.0);
    vec3 wld_vert_pos = wld_vert_pos4.xyz / wld_vert_pos4.w;
    vec4 wld_eye_pos4 = view_inv * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 wld_eye_pos = wld_eye_pos4.xyz / wld_eye_pos4.w;

    gl_Position = projection * wld_vert_pos4;
    outNormal = normalize(mat3x3(model) * inNormal);
    outColor = inColor;
    outEyeDir = normalize(wld_eye_pos - wld_vert_pos);
}
