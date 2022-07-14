#version 410 

layout (location=0) in vec3 inPos;
layout (location=2) in vec3 inColor;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    fragColor = inColor;
    gl_Position = proj * view * model * vec4(inPos,  1.0);
}