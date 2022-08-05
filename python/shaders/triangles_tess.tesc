#version 410 
layout (vertices = 3) out;

// attributes of the input CPs
in vec3 vNormal[];
in vec3 vCenter[];
in float vRadius[];
in vec3 vFragColor[];
in float level[];

// attributes of the output CPs
out vec3 tcNormal[];
out vec3 tcCenter[];
out float tcRadius[];
out vec3 tcFragColor[];

void main()
{
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    tcCenter[gl_InvocationID] = vCenter[gl_InvocationID];
    tcRadius[gl_InvocationID] = vRadius[gl_InvocationID];
    tcFragColor[gl_InvocationID] = vFragColor[gl_InvocationID];

    gl_TessLevelOuter[0] = (level[1] + level[2]) * 0.5;
    gl_TessLevelOuter[1] = (level[0] + level[2]) * 0.5;;
    gl_TessLevelOuter[2] = (level[0] + level[1]) * 0.5;;
    gl_TessLevelInner[0] = (level[0] + level[1] + level[2])/3;
}