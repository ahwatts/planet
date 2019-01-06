#version 430 core

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 inNormal;

uniform uint specular_exp;

layout(location = 0) out vec4 outColor;

void main(void) {
    outColor = vec4(inColor.rgb * inNormal.z, 1.0);
}
