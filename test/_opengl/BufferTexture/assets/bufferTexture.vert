#version 330 core

layout(location = 0) in int index;

out vec3 oColor;

uniform mat4 ciModelViewProjection;
uniform samplerBuffer positionColors;

void main() {
	vec3 position = texelFetch( positionColors, gl_VertexID * 2 + 0 ).xyz;
	vec3 color = texelFetch( positionColors, gl_VertexID * 2 + 1 ).rgb;

	gl_Position = ciModelViewProjection * vec4( position, 1.0 );
	oColor = color;
}