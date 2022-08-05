#version 410 
layout(quads) in;

in vec3 tcNormal[];
in vec3 tcCenter[];
in float tcRadius[];
in vec3 tcColor[];

out vec3 tePosition;
out vec3 teNormal;
out vec3 teColor;

uniform mat4 proj;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 centerA = mix(tcCenter[0], tcCenter[1], u);
    vec3 centerB = mix(tcCenter[2], tcCenter[3], u);
    vec3 center = mix(centerA, centerB, v);

    float radiusA = mix(tcRadius[0], tcRadius[1], u);
    float radiusB = mix(tcRadius[2], tcRadius[3], u);
    float radius = mix(radiusA, radiusB, v);

    vec3 normalA = normalize(mix(tcNormal[0], tcNormal[1], u));
    vec3 normalB = normalize(mix(tcNormal[2], tcNormal[3], u));
    teNormal = normalize(mix(normalA, normalB, v));
    
    vec3 colorA = mix(tcColor[0], tcColor[1], u);
    vec3 colorB = mix(tcColor[2], tcColor[3], u);
    teColor = mix(colorA, colorB, v);

    tePosition = center + teNormal * radius;

    gl_Position = proj * vec4(tePosition, 1.0);
}