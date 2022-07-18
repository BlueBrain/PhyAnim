#version 410

in vec3 position;
in vec3 normal;
in vec3 fragColor;

out vec4 oColor;

void main()
{
    vec3 L = normalize(-1.0 * position);

    float diff = clamp(dot(L,normal), 0, 1);

    oColor = vec4(fragColor*(diff*0.8+0.2), 1.0);
}