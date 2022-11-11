#version 410

in vec3 tePosition;
in vec3 teNormal;
in vec3 teColor;

out vec4 oColor;

void main()
{
    vec3 L = normalize(-1.0 * tePosition);

    float diff = clamp(dot(L, teNormal), 0, 1);

    vec3 c = teColor;

    oColor = vec4(c*(diff*0.8+0.2), 1.0);
}