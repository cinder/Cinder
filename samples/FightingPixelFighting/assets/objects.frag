#version 150

in vec4 vertPosition;
in vec3 vertNormal;
in vec4 vertColor;

out vec4 fragColor;

void main( void )
{
	// Simple diffuse lighting.
	vec3 N = normalize( vertNormal );
	vec3 L = normalize( -vertPosition.xyz );
	float NdotL = max( 0.0, dot( N, L ) );

	fragColor.rgb = vertColor.rgb * NdotL;
	fragColor.a = vertColor.a;
}