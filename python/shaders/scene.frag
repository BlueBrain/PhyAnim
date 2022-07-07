#version 330 core

in vec3 position;
in vec3 normal;
in vec3 fragColor;

out vec4 color;

void main()
{
    vec3 L = -1.0 * position;
    float diff = dot(L,normal);

    color = vec4(fragColor*diff, 1.0);
}