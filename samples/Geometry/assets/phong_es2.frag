#version 100

#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform sampler2D	uTex0;
uniform int			uTexturingMode;

varying vec4 Position;
varying vec3 Normal;
varying vec4 Color;
varying vec2 TexCoord;

// Based on OpenGL Programming Guide (8th edition), p 457-459.
highp float checkered( in vec2 uv, in int freq )
{
	highp vec2 checker = fract( uv * float( freq ) );
	highp vec2 edge = fwidth( uv ) * float( freq );
	highp float mx = max( edge.x, edge.y );

	highp vec2 pattern = smoothstep( vec2(0.5), vec2(0.5) + edge, checker );
	pattern += 1.0 - smoothstep( vec2(0.0), edge, checker );

	highp float factor = pattern.x * pattern.y + ( 1.0 - pattern.x ) * ( 1.0 - pattern.y );
	return mix( factor, 0.5, smoothstep( 0.0, 0.75, mx ) );
}

void main()
{
	// set diffuse and specular colors
	vec3 cDiffuse = Color.rgb;
	vec3 cSpecular = vec3( 0.3 );

	// light properties in view space
	vec3 vLightPosition = vec3( 0.0 );

	// lighting calculations
	vec3 N = normalize( Normal );
	vec3 L = normalize( vLightPosition - Position.xyz );
	vec3 E = normalize( -Position.xyz );
	vec3 H = normalize( L + E );

	// Calculate coefficients.
	float phong = max( dot( N, L ), 0.0 );

	const float kMaterialShininess = 20.0;
	const float kNormalization = ( kMaterialShininess + 8.0 ) / ( 3.14159265 * 8.0 );
	float blinn = pow( max( dot( N, H ), 0.0 ), kMaterialShininess ) * kNormalization;

	// diffuse coefficient
	vec3 diffuse = vec3( phong );

	if( uTexturingMode == 1 ) {
		diffuse *= vec3( 0.7, 0.5, 0.3 );
		diffuse *= 0.5 + 0.5 * checkered( TexCoord, 20 );
	}
	else if ( uTexturingMode == 2 )
		diffuse *= texture2D( uTex0, TexCoord.st ).rgb;

	// specular coefficient 
	vec3 specular = blinn * cSpecular;

	// alpha 
	float alpha = ( uTexturingMode == 3 ) ? 0.75 : 1.0;

	// final color
	gl_FragColor = vec4( diffuse + specular, alpha );
}
