#version 410 

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec3 inColor;

out vec3 vNormal;
out vec3 vCenter;
out float vRadius;
out vec3 vColor;
out float level;

uniform vec3 color;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;
uniform float maxLevel;
uniform float maxDistance;

void main()
{
    vec3 position = (view * model * vec4(inPos, 1.0)).xyz;
    vNormal = (view * model * vec4(inNormal, 0.0)).xyz;
    vCenter = position - vNormal;
    vRadius = length(vNormal);
    vColor = inColor;
    vNormal = normalize(vNormal);


    float minLevel = 1;
    float maxL = max(maxLevel, minLevel);
    float alpha = clamp(-position.z/maxDistance, 0.0, 1.0);
    level = (minLevel * alpha + maxL * (1-alpha)) * max(vRadius*0.2,1.0);
}