#version 410

flat in int vIndex;

uniform float uNumCubes;

out vec4 FragColor;

void main()
{
	FragColor = vec4(1., float( vIndex) / uNumCubes , 0., 1.0 );
}