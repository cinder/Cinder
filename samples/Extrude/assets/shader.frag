#version 150

in vec4		Color;
in vec3		Normal;

out vec4 	oColor;

void main( void )
{
	vec3 normal = normalize( Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, 1 ) ), 0 );
	oColor = vec4( vec3( Color ) * diffuse, Color.a );
}