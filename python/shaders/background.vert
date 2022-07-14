#version 410 

layout (location=0) in vec3 inPos;
layout (location=2) in vec3 inColor;

out vec3 fragColor;

void main()
{
    fragColor = inColor;
    gl_Position = vec4(inPos,  1.0);
}