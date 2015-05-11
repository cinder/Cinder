#version 300 es

in highp int index;

out mediump vec3 oColor;

uniform mediump mat4 ciModelViewProjection;

const vec3 positions[] = vec3[](
	vec3( -1.0,  1.0, 0.0 ),
	vec3( -1.0, -1.0, 0.0 ),
	vec3(  1.0, -1.0, 0.0 ),
	vec3( -1.0,  1.0, 0.0 ),
	vec3(  1.0, -1.0, 0.0 ),
	vec3(  1.0,  1.0, 0.0 )
);

const vec3 colors[] = vec3[](
	vec3( 1.0, 0.0, 0.0 ),
	vec3( 0.0, 1.0, 0.0 ),
	vec3( 0.0, 0.0, 1.0 ),
	vec3( 1.0, 0.0, 0.0 ),
	vec3( 0.0, 0.0, 1.0 ),
	vec3( 1.0, 1.0, 1.0 )
);

void main() {
	gl_Position = ciModelViewProjection * vec4( positions[index], 1.0 );
	oColor = colors[index];
}