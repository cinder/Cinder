#version 430

uniform mat4	ciModelMatrix;
uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;
in vec4		ciColor;

layout( std430, binding = 0 ) buffer Pos
{
	vec4 positions[];
};

layout (std140, binding = 1) uniform uCamMatrices {
	mat4 view;
	mat4 projection;
};

out highp vec2	TexCoord;
out lowp vec4	Color;
out highp vec3	Normal;

void main( void )
{
#ifdef ASYMMETRICAL
	gl_Position	= projection * view * ciModelMatrix * ( ciPosition + vec4( positions[gl_InstanceID].xyz, 0) );
#else
	gl_Position	= ciModelViewProjection * ( ciPosition + vec4( positions[gl_InstanceID].xyz, 0) );
#endif
	Color 		= ciColor;
	TexCoord	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
