#version 120
#extension GL_EXT_draw_instanced : enable

uniform float		INSTANCES_PER_ROW;

uniform sampler2D	tex0;

varying vec4	V;
varying vec3	N;

void main()
{
	// determine position for this hexagon
	float x = mod( gl_InstanceID, INSTANCES_PER_ROW );
	float y = floor( gl_InstanceID / INSTANCES_PER_ROW );

	// sample color from image
	vec2 pixel = vec2( 1.0 - x / INSTANCES_PER_ROW, 1.0 - y / (3 * INSTANCES_PER_ROW) );
	vec4 clr = texture2D( tex0, pixel );

	// convert to luminance and calculate corresponding angle in radians
	float luminance = dot( clr.rgb, vec3( 0.3, 0.59, 0.11 ) );
	float angle = acos( luminance );

	// create transform matrix
	mat4 m;
	m[0][0] = 1.0;
	m[1][1] = cos(angle);	m[2][1] = -sin(angle);
	m[1][2] = sin(angle);	m[2][2] =  cos(angle);
	m[3][0] = 3.0 * x + 1.5 * mod( y, 2.0 );
	m[3][1] = 0.866025 * y;
	m[3][3] = 1.0;

	// calculate the final vertex position
	V = gl_ModelViewMatrix * m * gl_Vertex;

	// do the same for the normal vector (note: this calculation is only correct if your model is NOT scaled!)
	N = normalize( gl_NormalMatrix * vec3( m * vec4( gl_Normal, 0.0 ) ) );
	
	gl_Position = gl_ProjectionMatrix * V;
}
