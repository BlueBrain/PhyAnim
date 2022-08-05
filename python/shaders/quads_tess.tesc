#version 410 
layout (vertices = 4) out;

// attributes of the input CPs
in vec3 vNormal[];
in vec3 vCenter[];
in float vRadius[];
in vec3 vColor[];
in float level[];

// attributes of the output CPs
out vec3 tcNormal[];
out vec3 tcCenter[];
out float tcRadius[];
out vec3 tcColor[];

#define ID gl_InvocationID

void main()
{
    tcNormal[ID] = vNormal[ID];
    tcCenter[ID] = vCenter[ID];
    tcRadius[ID] = vRadius[ID];
    tcColor[ID] = vColor[ID];

    float level0 = (level[2]+level[0])*0.5;
    float level1 = (level[0]+level[1])*0.5;
    float level2 = (level[1]+level[3])*0.5;
    float level3 = (level[3]+level[2])*0.5;
    float levelTotal = (level[0]+level[1]+level[2]+level[3])*0.25;

    gl_TessLevelOuter[0] = level0; // 3-0
    gl_TessLevelOuter[1] = level1; // 0-1
    gl_TessLevelOuter[2] = level2; // 1-2
    gl_TessLevelOuter[3] = level3; // 2-3
    gl_TessLevelInner[0] = levelTotal; // 0-1 && 3-2
    gl_TessLevelInner[1] = levelTotal; // 0-3 && 1-2
}