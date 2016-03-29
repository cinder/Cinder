#version 150
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set=1, binding=0) uniform sampler2D uTex0;
layout(set=2, binding=0) uniform sampler2D uTex1;
layout(set=3, binding=0) uniform sampler2D uTex2;

layout (location = 0) in vec2 TexCoord;

layout (location = 0) out vec4 oColor;

void main( void )
{
	const float dy = 1.0/3.0;

	if( TexCoord.y >= 2.0*dy ) {
		vec2 uv = vec2( TexCoord.x, (TexCoord.y - 2.0*dy)/dy );
		oColor = texture( uTex2, uv );		
	}
	else if( TexCoord.y >= dy ) {
		vec2 uv = vec2( TexCoord.x, (TexCoord.y - dy)/dy );
		oColor = texture( uTex1, uv );
	}
	else {
		vec2 uv = vec2( TexCoord.x, TexCoord.y/dy );
		oColor = texture( uTex0, uv );
	}
}
