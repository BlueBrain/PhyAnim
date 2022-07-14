#version 410

in vec3 fragColor;
out vec4 oColor;

void main()
{
    oColor = vec4(fragColor, 1.0);
}