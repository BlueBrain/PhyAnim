#version 330 core

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inColor;

out vec3 position;
out vec3 normal;
out vec3 fragColor;


void main()
{
    position = vec3(inPos.xy, -1.0);
    normal = inNormal;
    fragColor = inColor;
    gl_Position = vec4(inPos.xyz,  1.0);
}