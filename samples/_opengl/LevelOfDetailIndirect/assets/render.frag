#version 150

uniform mat4 ciViewMatrix;
uniform bool uVisualizeLods;

in vec4 Position;
in vec3 Normal;
flat in float Lod;

out vec4 oColor;

const vec3 kLightPosition = vec3( 0 );

void main()
{
	vec3 diffuseColor;

	// LOD-specific conditional shading goes here
	if( ! uVisualizeLods || Lod == 0 )
		diffuseColor = vec3( 0.85, 0.0, 0.15 );
	else if( Lod == 1 )
		diffuseColor = vec3( 0.15, 0.85, 0.0 );
	else if( Lod == 2 )
		diffuseColor = vec3( 0.15, 0.35, 0.85 );

	vec3 L = normalize( kLightPosition - Position.xyz );
	vec3 E = normalize( -Position.xyz );
	vec3 N = normalize( Normal );
	vec3 H = normalize( L + E );

	vec3 diffuse = diffuseColor * max( dot( N, L ), 0.0 );

	const float kShininess = 200.0;
	const float kNormalization = ( kShininess + 8.0 ) / ( 3.14159265 * 8.0 );
	const vec3 kSpecularColor = vec3( 1, 0.8, 0.8 );
	vec3 specular = kNormalization * kSpecularColor * pow( max( dot( N, H ), 0.0 ), kShininess );

	oColor.rgb = diffuse + specular;
	oColor.a = 1.0;
}
