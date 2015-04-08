#version 150

uniform sampler2D	uTex0;

in vec3		vNormal;
in vec2		vTexCoord0;

out vec4 	oFragColor;

void main( void )
{
	vec3 normal = normalize( -vNormal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oFragColor = texture( uTex0, vTexCoord0 ) * diffuse;
}
