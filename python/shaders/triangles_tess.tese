#version 410 
layout(triangles, equal_spacing, ccw) in;


in vec3 tcNormal[];
in vec3 tcCenter[];
in float tcRadius[];
in vec3 tcFragColor[];

out vec3 position;
out vec3 normal;
out vec3 fragColor;

uniform mat4 proj;

void main()
{
    vec3 center = gl_TessCoord.x * tcCenter[0]+
        gl_TessCoord.y * tcCenter[1]+
        gl_TessCoord.z * tcCenter[2];
    
    float radius = gl_TessCoord.x * tcRadius[0]+
        gl_TessCoord.y * tcRadius[1]+
        gl_TessCoord.z * tcRadius[2];

    normal = normalize(gl_TessCoord.x * tcNormal[0]+
        gl_TessCoord.y * tcNormal[1]+
        gl_TessCoord.z * tcNormal[2]);

    position = center + normal * radius;
   
    fragColor = gl_TessCoord.x * tcFragColor[0]+
        gl_TessCoord.y * tcFragColor[1]+
        gl_TessCoord.z * tcFragColor[2];;
    gl_Position = proj * vec4(position,  1.0);
}