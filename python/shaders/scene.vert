#version 410 

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inColor;

out vec3 position;
out vec3 normal;
out vec3 fragColor;

uniform vec3 color;
uniform mat4 uView;
uniform mat4 proj;

void main()
{
    position = (vec4(inPos, 1.0) * uView).xyz;
    normal = inNormal;
    fragColor = color;
    gl_Position = proj * vec4(position,  1.0);
}