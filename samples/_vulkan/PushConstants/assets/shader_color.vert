#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform ciBlock0T {
	mat4 	ciModelViewProjection;
	vec2	ciRect[2];
	vec2	ciTexCoord[2];
	vec4	ciColor;
} ciBlock0;

layout(location = 0) in vec4 ciPosition;

layout(location = 0) out vec4 Color;

void main( void )
{
	float x = mix( ciBlock0.ciRect[0].x, ciBlock0.ciRect[1].x, ciPosition.x );
	float y = mix( ciBlock0.ciRect[0].y, ciBlock0.ciRect[1].y, ciPosition.y );
	vec4 pos = vec4( x, y, ciPosition.zw );
	gl_Position	  = ciBlock0.ciModelViewProjection * pos;

	Color = ciBlock0.ciColor;
}
