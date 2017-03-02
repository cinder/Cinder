#version 330 core

in vec2		vTexCoord0;
in vec2		vTexCoord1;
in vec3		vNormal;

layout (location = 0) out vec4 oFragColor0;
layout (location = 1) out vec4 oFragColor1;

void main()
{
	// draw green to attachment 0, blue to attachment 1
	vec3 colorAttachment0 = vec3( 0, 1, 0 );
	vec3 colorAttachment1 = vec3( 0, 0, 1 );

	// add some basic shading
	const vec3 lightPos = vec3( 0, 0, 1 );
	float lambert = max( 0.0, dot( normalize( vNormal ), lightPos ) );

	colorAttachment0 *= lambert;
	colorAttachment1 *= lambert;

	oFragColor0 = vec4( colorAttachment0, 1.0 );
	oFragColor1 = vec4( colorAttachment1, 1.0 );
}
