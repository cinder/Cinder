#version 150

in vec4	vertPosition;
in vec3	vertNormal;

out vec4 fragColor;

void main()
{
	// Light position in view space.
	const vec3 kLightPosition = vec3( 0 );

	// Calculate lighting vectors.
	vec3 L = normalize( kLightPosition - vertPosition.xyz );
	vec3 E = normalize( -vertPosition.xyz );
	vec3 N = normalize( vertNormal );
	vec3 H = normalize( L + E );

	// Calculate diffuse lighting component.
	const vec3 kDiffuseColor = vec3( 1, 0, 0 );
	vec3 diffuse = kDiffuseColor * max( dot( N, L ), 0.0 );

	// Calculate specular lighting component.
	const float kShininess = 200.0;
	const float kNormalization = ( kShininess + 8.0 ) / ( 3.14159265 * 8.0 );
	const vec3 kSpecularColor = vec3( 1, 0.8, 0.8 );
	vec3 specular = kNormalization * kSpecularColor * pow( max( dot( N, H ), 0.0 ), kShininess );

	// Output final color.
	fragColor = vec4( diffuse + specular, 1.0 );
}
