#version 430

uniform sampler2D uTex0;
layout (std140, binding=0) uniform uMaterial {
	vec3 uColor;
};

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord;

out vec4 	oColor;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
	oColor = texture( uTex0, TexCoord.st ) * Color * diffuse;
	oColor.rgb *= uColor.rgb;
	oColor.a = 1.0;
}