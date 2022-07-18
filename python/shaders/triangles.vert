#version 410 

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inColor;

out vec3 position;
out vec3 normal;
out vec3 fragColor;

uniform vec3 color;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    position = (view * model * vec4(inPos, 1.0)).xyz;
    normal = (view*model*vec4(normalize(inNormal),0)).xyz;
    fragColor = inColor;
    gl_Position = proj * vec4(position,  1.0);
}