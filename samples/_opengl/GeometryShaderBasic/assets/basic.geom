#version 150

layout(points) in;
layout(line_strip, max_vertices = 64) out;

in vec3 vColor[]; // Output from vertex shader for each vertex
out vec3 gColor; // Output to fragment shader

uniform int uNumSides;
uniform float uRadius;

uniform mat4 ciProjectionMatrix;

const float PI = 3.1415926;

void main()
{
	gColor = vColor[0];
	
	for( int i = 0; i <= uNumSides; i++ ) {
		float ang = PI * 2.0 / uNumSides * i;
		vec4 offset = vec4( cos(ang) * uRadius, -sin(ang) * uRadius, 0.0, 0.0 );
		gl_Position = ciProjectionMatrix * (gl_in[0].gl_Position + offset);
		EmitVertex();
	}
	
	EndPrimitive();
}