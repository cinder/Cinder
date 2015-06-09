#version 150

// we expect the model to be made of triangles (GL_TRIANGLES)
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

// we will output the distance to each vertex
noperspective out vec3 vDistance;

// we need to know the size of the viewport in pixels (x = width, y = height)
uniform vec2 uViewportSize;

void main()
{
	// taken from 'Single-Pass Wireframe Rendering'
	vec2 p0 = uViewportSize * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
	vec2 p1 = uViewportSize * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
	vec2 p2 = uViewportSize * gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

	vec2 v0 = p2 - p1;
	vec2 v1 = p2 - p0;
	vec2 v2 = p1 - p0;
	float area = abs( v1.x * v2.y - v1.y * v2.x );

	vDistance = vec3( area / length( v0 ), 0,0 );
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	vDistance = vec3( 0, area / length( v1 ), 0 );
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	vDistance = vec3( 0, 0, area / length( v2 ) );
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}