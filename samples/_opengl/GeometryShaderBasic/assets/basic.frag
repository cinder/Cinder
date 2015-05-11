#version 150

in vec3 gColor;

out vec4 outColor;

void main()
{
    outColor = vec4(gColor, 1.0);
}