#version 100

precision highp float;

varying vec4 Position;
varying vec3 Normal;
varying vec4 Color;
varying vec2 TexCoord;

void main()
{
	// set diffuse and specular colors
	vec3 cDiffuse = Color.rgb;
	vec3 cSpecular = vec3( 0.3 );

	// light properties in view space
	vec3 vLightPosition = vec3( 0.0 );

	// lighting calculations
	vec3 vVertex = Position.xyz;
	vec3 vNormal = normalize( Normal );
	vec3 vToLight = normalize( vLightPosition - vVertex );
	vec3 vToEye = normalize( - vVertex );
	vec3 vReflect = normalize( - reflect( vToLight, vNormal ) );

	// diffuse coefficient
	vec3 diffuse = max( dot( vNormal, vToLight ), 0.0 ) * cDiffuse;

	// texCoord checkerboard
	if( mod( floor( TexCoord.x * 20.0 ) + floor( TexCoord.y * 20.0 + 0.001 ), 2.0 ) == 0.0 )
		diffuse *= 0.5;

	// texCoord checkerboard with protection against edge case\n"
	const float kEpsilon = 0.0001;
	vec2 p = TexCoord * 10.0;
	float x = 0.5 - 0.5 * step( min( fract( p.x - kEpsilon ), fract( p.x + kEpsilon ) ), 0.5 );
	float y = 0.5 + 0.5 * step( min( fract( p.y - kEpsilon ), fract( p.y + kEpsilon ) ), 0.5 );
	diffuse *= 0.5 + fract( x + y );

	// specular coefficient with energy conservation
	const float shininess = 20.0;
	const float coeff = (2.0 + shininess) / (2.0 * 3.14159265);
	vec3 specular = pow( max( dot( vReflect, vToEye ), 0.0 ), shininess ) * coeff * cSpecular;

	// to conserve energy, diffuse and specular colors should not exceed one
	float maxDiffuse = max( diffuse.r, max( diffuse.g, diffuse.b ) );
	float maxSpecular = max( specular.r, max( specular.g, specular.b ) );
	float fConserve = 1.0 / max( 1.0, maxDiffuse + maxSpecular );

	// final color
	gl_FragColor = vec4( (diffuse + specular) * fConserve, 1.0 );
}
