#version 420

in vec3 vColor;
out vec4 outColor;

void main()
{
    outColor = vec4( vColor, 1.0 );
}