#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(shared) uniform ViewAndProjectionBlock {
    mat4x4 view;
    mat4x4 view_inv;
    mat4x4 projection;
};

uniform mat4x4 model;

layout(location = 0) out float outHeight;
layout(location = 1) out vec3 outNormal;

void main(void) {
    // Position, in "world" coordinates, of the vertex.
    vec4 wld_position4 = model * vec4(inPosition, 1.0);
    vec3 wld_position = wld_position4.xyz / wld_position4.w;

    // Position, in "world" coordinates, of the eye (i.e,
    // un-transform the origin of the "eye" coordinates).
    // vec4 wld_eye_position4 = view_inv * vec4(0.0, 0.0, 0.0, 1.0);
    // vec3 wld_eye_position = wld_eye_position4.xyz / wld_eye_position4.w;

    // Direction, in "world" cordinates, of the normal at this vertex. This
    // should use the inverse transpose of the upper-left 3x3 of model, but we'e
    // not doing any nonuniform scaling (right?)
    vec3 wld_normal = normalize(mat3(model) * inNormal);

    // Set output variables.
    gl_Position = projection * view * wld_position4;
    outHeight = length(inPosition);
    outNormal = wld_normal;
}
