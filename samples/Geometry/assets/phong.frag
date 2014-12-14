#version 150

uniform sampler2D	uTex0;
uniform int			uTexturingMode;

in VertexData	{
	vec4 position;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vVertexIn;

out vec4 oFragColor;

// Based on OpenGL Programming Guide (8th edition), p 457-459.
float checkered( in vec2 uv, in int freq )
{
	vec2 checker = fract( uv * freq );
	vec2 edge = fwidth( uv ) * freq;
	float mx = max( edge.x, edge.y );

	vec2 pattern = smoothstep( vec2(0.5), vec2(0.5) + edge, checker );
	pattern += 1.0 - smoothstep( vec2(0.0), edge, checker );

	float factor = pattern.x * pattern.y + ( 1.0 - pattern.x ) * ( 1.0 - pattern.y );
	return mix( factor, 0.5, smoothstep( 0.0, 0.75, mx ) );
}

void main()
{
	// set diffuse and specular colors
	vec3 cDiffuse = vVertexIn.color.rgb;
	vec3 cSpecular = vec3( 0.3 );

	// light properties in view space
	vec3 vLightPosition = vec3( 0.0 );

	// lighting calculations
	vec3 vVertex = vVertexIn.position.xyz;
	vec3 vNormal = normalize( vVertexIn.normal );
	vec3 vToLight = normalize( vLightPosition - vVertex );
	vec3 vToEye = normalize( -vVertex );
	vec3 vReflect = normalize( -reflect(vToLight, vNormal) );

	// diffuse coefficient
	vec3 diffuse = max( dot( vNormal, vToLight ), 0.0 ) * cDiffuse;

	if( uTexturingMode == 1 )
		diffuse *= 0.5 + 0.5 * checkered( vVertexIn.texCoord, 20 );
	else if ( uTexturingMode == 2 )
		diffuse *= 0.5 + 0.5 * texture( uTex0, vVertexIn.texCoord.st ).rgb;

	// specular coefficient with energy conservation
	const float shininess = 20.0;
	const float coeff = (2.0 + shininess) / (2.0 * 3.14159265);
	vec3 specular = pow( max( dot( vReflect, vToEye ), 0.0 ), shininess ) * coeff * cSpecular;

	// to conserve energy, diffuse and specular colors should not exceed one
	float maxDiffuse = max( diffuse.r, max( diffuse.g, diffuse.b ) );
	float maxSpecular = max( specular.r, max( specular.g, specular.b ) );
	float fConserve = 1.0 / max( 1.0, maxDiffuse + maxSpecular );

	// final color
	oFragColor = vec4( (diffuse + specular) * fConserve, 1.0 );
}
