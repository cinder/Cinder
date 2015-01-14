#version 150

in vec3		PositionEyeSpace;
in vec3		Normal;

uniform vec3	uDiffuse;
uniform vec3	uSpecular;
uniform float	uShininess;

out vec4 oColor;

void main()
{
	// light properties in view space
	const vec3 lightPos = vec3( 0.0 );

	vec3 toLight = normalize( lightPos - PositionEyeSpace );
	vec3 toEye = normalize( -PositionEyeSpace );
	vec3 refl = normalize( reflect( -toLight, Normal ) );

	vec3 diffuse = uDiffuse * max( dot( Normal, toLight ), 0.0 );

	vec3 specular = uSpecular * pow( max( dot( refl, toEye ), 0.0 ), uShininess );
	specular = clamp( specular, 0.0, 1.0 );

	oColor = vec4( diffuse + specular, 1.0 );
}
